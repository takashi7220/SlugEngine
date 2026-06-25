# SlugEngine C++ コーディング規約

## 目次

1. [基本方針](#1-基本方針)
2. [命名規則](#2-命名規則)
3. [ファイル構成](#3-ファイル構成)
4. [namespace](#4-namespace)
5. [クラス設計](#5-クラス設計)
6. [テンプレート・コンセプト](#6-テンプレートコンセプト)
7. [修飾子の使い方](#7-修飾子の使い方)
8. [メモリ管理](#8-メモリ管理)
9. [マクロ](#9-マクロ)
10. [コメント](#10-コメント)
11. [フォーマット](#11-フォーマット)

---

## 1. 基本方針

- C++20 を使用する。
- MSVC (Windows) をプライマリコンパイラとする。
- STL は原則として `slug::core` の型エイリアスを通じて使用する。
- RAII を徹底し、生のリソース管理は行わない。
- 例外は使用しない。エラー通知はアサーションまたは戻り値で行う。

---

## 2. 命名規則

### 2.1 クラス / 構造体 / 列挙型

**PascalCase** を使用する。

```cpp
class ReferenceObject { };
struct ControlBlock { };
struct AdoptRefT { };
enum class LogType : uint8_t { Info, Warning, Error };
```

### 2.2 テンプレートクラス / 型エイリアス

テンプレートクラスおよびテンプレート型エイリアスには **`T` プリフィックス** を付ける。

```cpp
template<typename T> class TReferencePtr { };
template<typename T> class TWeakReferencePtr { };
template<typename T> class TUniquePtr { };
template<typename T> using TAtomic = std::atomic<T>;
template<typename T> using TVector = std::vector<T, TAllocator<T>>;
template<typename T> using TAllocator = ...;
```

非テンプレートの型エイリアスは `T` を付けない。

```cpp
using String = std::basic_string<char, std::char_traits<char>, TAllocator<char>>;
using ObjectType = uint32_t;
```

### 2.3 関数・メソッド

**PascalCase** を使用する。

```cpp
// メンバ関数
virtual unsigned long AddRef() noexcept;
virtual unsigned long Release() noexcept;
bool TryAddRefFromWeak() noexcept;
T* get() const noexcept;           // 例外: STL 互換の getter は小文字
void reset() noexcept;             // 例外: STL 互換操作は小文字
void swap(TReferencePtr& r) noexcept;

// グローバル / 静的ファクトリ関数
template<HasAddRefAndRelease T, class... Args>
TReferencePtr<T> MakeReference(Args&&... args);

template<typename T>
TUniquePtr<T> MakeUnique();

static TReferencePtr create(T* other) noexcept;
```

> **例外**: `get()`, `reset()`, `swap()`, `detach()`, `attach()` など STL の慣習に合わせるメソッドは lowercase とする。

### 2.4 メンバ変数

**`m_` プリフィックス + camelCase** を使用する。

```cpp
private:
    T* m_ptr = nullptr;
    ControlBlock* m_control = nullptr;
    core::TVector<uint8_t>& m_buffer;
```

`ControlBlock` のような値型 struct の公開フィールドはプリフィックスなし（snake_case）。

```cpp
struct ControlBlock : public MemoryObject
{
    TAtomic<unsigned long> strong { 0 };
    TAtomic<unsigned long> weak { 1 };
    TAtomic<bool> alive { true };
    TAtomic<void*> object_ptr { nullptr };
};
```

### 2.5 テンプレートパラメータ

単一型パラメータは **`T`**、複数の場合は **意味のある名前** を PascalCase で付ける。

```cpp
template<typename T>
class TReferencePtr { };

template<typename Archive, typename T>
void DispatchSerialize(Archive& ar, const T& value);
```

### 2.6 namespace

**snake_case** を使用する。

```cpp
namespace slug { }
namespace slug::core { }
```

### 2.7 概念 (Concept)

**PascalCase** を使用する。

```cpp
template<typename T>
concept ReferenceObjectType = std::derived_from<T, ReferenceObject>;

template<typename T>
concept HasAddRefAndRelease = requires(T t)
{
    { t.AddRef() } -> std::same_as<unsigned long>;
    { t.Release() } -> std::same_as<unsigned long>;
};

template<typename T>
concept BoolType = std::same_as<remove_cvref_t<T>, bool>;

template<typename T>
concept IntengerType = is_arithmetic_or_enum_v<T> &&
    !std::same_as<remove_cvref_t<T>, bool> &&
    !std::is_floating_point_v<remove_cvref_t<T>>;
```

### 2.8 マクロ

**`SLUG_` プリフィックス + UPPER_SNAKE_CASE** を使用する（[9. マクロ](#9-マクロ) 参照）。

### 2.9 列挙値

**PascalCase** を使用する。

```cpp
enum class LogType : uint8_t
{
    Info = 0,
    Warning,
    Error,
    Assert,
    Alert,
    Fatal
};
```

---

## 3. ファイル構成

### 3.1 インクルードガード

`#pragma once` **のみ** を使用する。`#ifndef` ガードは使用しない。

```cpp
#pragma once

#include <cstdint>
#include "core/Define.hpp"
```

### 3.2 ヘッダ / ソース分離

テンプレートクラスとインライン関数は **ヘッダファイルのみ** に実装する。
実装が大きい非テンプレートクラスは `src/` にソースファイルを置く。

### 3.3 ディレクトリ構成

機能カテゴリごとにサブディレクトリを切る。

```
libs/core/
├── include/core/
│   ├── memory/       # メモリ管理
│   ├── container/    # コンテナ
│   ├── thread/       # スレッド
│   ├── serialize/    # シリアライズ
│   │   ├── archiver/archives/binary/
│   │   ├── archiver/archives/json/
│   │   ├── archiver/types/
│   │   └── common/
│   ├── reflection/   # リフレクション
│   ├── debug/        # デバッグ
│   └── ...
└── src/              # 非テンプレート実装
```

### 3.4 インクルード順序

1. 対応するヘッダ（実装ファイルの場合）
2. 標準ライブラリ（`<cstdint>`, `<type_traits>` 等）
3. エンジン内部ヘッダ（`"core/..."` 等）

---

## 4. namespace

### 4.1 宣言スタイル

**C++17 スコープ付き namespace** を使用する。ネスト形式は使用しない。

```cpp
namespace slug::core
{
    class MyClass { };
}

namespace slug::rhi::d3d12
{
    class Device { };
}
```

### 4.2 インデント

namespace のスコープ内はインデント **しない**。

```cpp
namespace slug::core
{

class MyClass
{
};

}
```

### 4.3 閉じ括弧コメント

閉じ括弧へのコメントは **付けない**。

---

## 5. クラス設計

### 5.1 アクセス修飾子の順序

**`public` → `protected` → `private`** の順に記述する。

```cpp
class ReferenceObject : public MemoryObject
{
public:
    // 公開 API
    virtual unsigned long AddRef() noexcept;
    virtual unsigned long Release() noexcept;

protected:
    ReferenceObject();
    virtual ~ReferenceObject() override;

private:
    ControlBlock* m_control;
};
```

### 5.2 コピー・ムーブの明示

コピー / ムーブを禁止する場合は `= delete`、デフォルト動作を使う場合は `= default` を **明示的に** 記述する。

```cpp
// コピー・ムーブ禁止
class ReferenceObject
{
public:
    ReferenceObject(const ReferenceObject&) = delete;
    ReferenceObject(ReferenceObject&&) = delete;
    ReferenceObject& operator=(const ReferenceObject&) = delete;
    ReferenceObject& operator=(ReferenceObject&&) = delete;
};

// デフォルト動作を使用
class TReferencePtr
{
public:
    TReferencePtr() noexcept = default;
};
```

### 5.3 継承

継承は `public` 継承を原則とする。

```cpp
class BinaryOutputArchive : public OutputArchive<BinaryOutputArchive> { };
struct ControlBlock : public MemoryObject { };
```

すべてのオブジェクトは `MemoryObject` を継承し、カスタムアロケータを使用する。

### 5.4 純粋仮想クラス

インターフェースは純粋仮想関数のみで定義し、`I` プリフィックスを付ける。

```cpp
class ILogCategory
{
public:
    virtual core::StringView GetName() const = 0;
    virtual ~ILogCategory() = default;
};
```

---

## 6. テンプレート・コンセプト

### 6.1 `typename` vs `class`

**`typename` を優先** する。可変引数テンプレートには `class...` も許容。

```cpp
template<typename T>
class TReferencePtr { };

template<typename Archive, typename T>
void DispatchSerialize(Archive& ar, const T& value);

template<typename T, class... Args>
TReferencePtr<T> MakeReference(Args&&... args);
```

### 6.2 コンセプトによる制約

型制約には C++20 **Concept** を使用する。`std::enable_if` は既存互換が必要な場合のみ許容。

```cpp
// 推奨
template<HasAddRefAndRelease T, class... Args>
TReferencePtr<T> MakeReference(Args&&... args);

// 互換性が必要な場合のみ
template<class U>
TReferencePtr(TReferencePtr<U>&& other,
    typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type* = nullptr) noexcept;
```

### 6.3 型特性

型特性は `_v` サフィックスの `inline constexpr` 変数テンプレートで公開する。

```cpp
template<typename T>
struct is_vector : std::false_type { };

template<typename T, typename Alloc>
struct is_vector<core::TVector<T, Alloc>> : std::true_type { };

template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;
```

### 6.4 SFINAE による関数存在確認

`std::void_t` と部分特殊化を使う。

```cpp
template<typename Archive, typename T, typename = void>
struct has_member_serialize : std::false_type { };

template<typename Archive, typename T>
struct has_member_serialize<Archive, T, std::void_t<
    decltype(std::declval<const T&>().Serialize(std::declval<Archive&>()))>>
    : std::true_type { };
```

---

## 7. 修飾子の使い方

### 7.1 `noexcept`

例外を投げない関数には **必ず** `noexcept` を付ける。コンストラクタ・デストラクタ・ムーブ操作・アクセサは原則 `noexcept`。

```cpp
TReferencePtr() noexcept = default;
TReferencePtr(TReferencePtr&& other) noexcept;
~TReferencePtr() noexcept;
T* get() const noexcept;
void swap(TReferencePtr& r) noexcept;
virtual unsigned long AddRef() noexcept;
```

### 7.2 `const`

読み取り専用のメンバ関数には **必ず** `const` を付ける。

```cpp
T* get() const noexcept;
unsigned long GetReferenceCount() const noexcept;
void* GetObjectPtrAcquire() const noexcept;
bool expired() const noexcept;
```

### 7.3 `override`

仮想関数をオーバーライドする際は **必ず** `override` を付ける。

```cpp
virtual ~ReferenceObject() override;
virtual void BeginObject() override;
virtual core::StringView GetName() const override;
```

`override` が付いている場合、`virtual` は省略しても良いが、明示する方が読みやすい。

### 7.4 `[[nodiscard]]` / `SLUG_NODISCARD`

戻り値を必ず使用すべき関数には `SLUG_NODISCARD` を付ける。

```cpp
SLUG_NODISCARD T* get() const noexcept;
SLUG_NODISCARD T** get_address_of() noexcept;
SLUG_NODISCARD T** release_and_get_address_of() noexcept;
```

### 7.5 `constexpr` / `inline constexpr`

コンパイル時定数・型特性変数には `inline constexpr` を使用する。

```cpp
inline constexpr AdoptRefT adoptRef {};
template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;
```

---

## 8. メモリ管理

### 8.1 `MemoryObject` 継承

ヒープ確保されるすべてのオブジェクトは **`MemoryObject`** を継承する。`MemoryObject` はカスタムアロケータ（mimalloc ベース）を `operator new / delete` でフックする。

```cpp
class ReferenceObject : public MemoryObject { };
struct ControlBlock : public MemoryObject { };
```

### 8.2 スマートポインタの使用

| ポインタ型 | 用途 |
|---|---|
| `TReferencePtr<T>` | 共有所有（参照カウント） |
| `TWeakReferencePtr<T>` | 弱参照（ライフタイム非延長） |
| `TUniquePtr<T>` | 単独所有 |

生ポインタの所有権は持たない。`new` の結果は直ちにスマートポインタに渡す。

```cpp
// 良い例
TReferencePtr<MyObject> obj = MakeReference<MyObject>(args...);
TUniquePtr<MyData> data = MakeUnique<MyData>();

// 悪い例
MyObject* raw = new MyObject();  // 所有権が不明確
```

### 8.3 `MakeReference` / `MakeUnique`

ファクトリ関数を通じてオブジェクトを生成する。

```cpp
template<HasAddRefAndRelease T, class... Args>
TReferencePtr<T> MakeReference(Args&&... args)
{
    return TReferencePtr<T>(new T(std::forward<Args>(args)...));
}
```

### 8.4 `adoptRef` による所有権移譲

参照カウントを増やさずに生ポインタを採用する場合は `adoptRef` タグを使用する。

```cpp
TReferencePtr<T> ptr(rawPtr, adoptRef);
```

---

## 9. マクロ

### 9.1 命名

`SLUG_` プリフィックス + **UPPER_SNAKE_CASE**。

### 9.2 定義済みマクロ一覧

| マクロ | 用途 |
|---|---|
| `SLUG_NODISCARD` | `[[nodiscard]]` の別名 |
| `SLUG_MAYBE_UNUSED` | `[[maybe_unused]]` の別名 |
| `SLUG_DEPRECATED` | `[[deprecated]]` の別名 |
| `SLUG_DEPRECATED_MSG(msg)` | `[[deprecated(msg)]]` の別名 |
| `SLUG_FORCE_INLINE` | `__forceinline` の別名 |
| `SLUG_ASSERT(expr)` | デバッグアサーション |
| `SLUG_ASSERT_MSG(expr, msg)` | メッセージ付きアサーション |
| `SLUG_STATIC_ASSERT(expr)` | コンパイル時アサーション |
| `SLUG_NAMEOF(x)` | 文字列化 (`#x`) |
| `SLUG_PROPERTY(...)` | リフレクション用プロパティ宣言 |

### 9.3 使用方針

- C++ の言語機能（属性、`constexpr`、テンプレート）で実現できる場合はマクロを使わない。
- プラットフォーム固有の処理・コンパイラ拡張・コード生成に限りマクロを使用する。

---

## 10. コメント

### 10.1 コメント言語

日本語・英語どちらも許容する。プロジェクト全体で混在しても問題ない。

### 10.2 コメントの方針

- **何をするか** をコメントで書かない（コードが明示している）。
- **なぜそうするか**、**非自明な制約** のみをコメントする。

```cpp
// 良い例: 意図・制約を説明
void attach(T* other) noexcept
{
    if (m_ptr == other) return;
    InternalRelease();
    m_ptr = other; // attach は所有を "採用"（AddRef しない）
}

// 悪い例: コードの繰り返し
// m_ptr に other を代入する
m_ptr = other;
```

### 10.3 Doxygen

現状 Doxygen は使用しない。

---

## 11. フォーマット

### 11.1 インデント

**4スペース**（タブ不使用）。

### 11.2 波括弧

関数・クラス・制御構文の波括弧は **次の行** に置く（Allman スタイル）。

```cpp
class MyClass
{
public:
    void Method()
    {
        if (condition)
        {
            DoSomething();
        }
    }
};
```

### 11.3 1行の長さ

厳密な上限は設けないが、**120文字** を目安とする。

### 11.4 メンバ初期化子リスト

コンストラクタのメンバ初期化子リストは各メンバを **別行** に置く。

```cpp
ReferenceObject()
    : m_control(new ControlBlock(this))
{
}

TReferencePtr(const TReferencePtr& other) noexcept
    : m_ptr(other.m_ptr)
{
    InternalAddRef();
}
```

### 11.5 メンバ変数のインライン初期化

デフォルト値がある場合は宣言時に初期化する。

```cpp
private:
    T* m_ptr = nullptr;
    ControlBlock* m_cb = nullptr;
```

### 11.6 `requires` 句のフォーマット

Concept の `requires` ブロックは各制約式を個別の行に展開する。

```cpp
template<typename T>
concept HasAddRefAndRelease = requires(T t)
{
    {
        t.AddRef()
    } -> std::same_as<unsigned long>;
    {
        t.Release()
    } -> std::same_as<unsigned long>;
};
```
