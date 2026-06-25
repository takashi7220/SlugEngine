# リフレクションシステム設計

## 目次

1. [概要](#1-概要)
2. [ディレクトリ構成](#2-ディレクトリ構成)
3. [設計の柱](#3-設計の柱)
4. [コンポーネント詳細](#4-コンポーネント詳細)
5. [型登録フロー](#5-型登録フロー)
6. [実行時利用フロー](#6-実行時利用フロー)
7. [ユーザー型への対応](#7-ユーザー型への対応)
8. [実装状況・既知の課題](#8-実装状況既知の課題)

---

## 1. 概要

`core/reflection` は、C++ の型情報（プロパティ・メソッド・コンストラクタ）を実行時に問い合わせ・操作するためのリフレクションシステムである。

設計の根本方針は以下の 3 点。

- **RTTI 不使用**: 型識別はすべて型名文字列のハッシュ値（`TypeId = uint64_t`）で行う。`typeid` や `dynamic_cast` は使用しない。
- **コンパイル時バインディング**: プロパティ・メソッドのアクセサはテンプレートで生成したラムダとして格納され、実行時は関数オブジェクト呼び出しのみになる。仮想関数によるディスパッチは `Variant` の内部実装にのみ限定する。
- **非侵襲的な登録**: 対象クラスを変更することなく、外部の登録コードだけでリフレクション情報を追加できる。

---

## 2. ディレクトリ構成

```
reflection/
├── Attribute.hpp          # TypeId 定義・PropertyDescriptor（エディタ用メタデータ）
├── TypeResolver.hpp       # TypeOf<T>() エントリポイント・特殊化インターフェース
├── Type.hpp / Type.cpp    # 型メタデータ本体
├── Variant.hpp            # 型消去された値の所有コンテナ
├── Instance.hpp           # 既存オブジェクトへの非所有参照
├── Property.hpp           # プロパティメタデータ（getter/setter）
├── Method.hpp             # メソッドメタデータ（invoke）
├── Constructor.hpp        # コンストラクタメタデータ（invoke）
├── Registration.hpp       # ReflectionRegistry（グローバルシングルトン）
├── TypeRegister.hpp       # TypeRegister<T>（Fluent Builder による登録 API）
├── ReflectionUtility.hpp  # SLUG_DEFINE_TYPE_RESOLVER マクロ
└── ReflectionClass.hpp    # （予約・未実装）
```

利用側は用途に応じて `TypeResolver.hpp`（読み取り）または `ReflectionUtility.hpp`（登録）をインクルードする。

---

## 3. 設計の柱

### 3.1 TypeId — ハッシュベースの型識別

```cpp
using TypeId = uint64_t;
```

`TypeId` は型名文字列を `HashUtility::ConvertStringToHash64` でハッシュ化した値である。`TypeResolver<T>::Get()` が返す `Type` オブジェクトと、`Variant` / `Instance` が保持する `TypeId` を比較することで型の一致を判定する。

RTTI を使わないため、DLL 境界を跨いでも同じ型名であれば同一の `TypeId` が得られる。ただし、異なる型に同じ名前を与えるとハッシュ衝突が発生する。

### 3.2 TypeResolver — TypeOf\<T\>() の特殊化点

```cpp
template<typename T>
const Type& TypeOf()
{
    return TypeResolver<remove_cvref_t<T>>::Get();
}
```

`TypeOf<T>()` は `TypeResolver<T>` の特殊化を呼ぶ。デフォルト実装は `static_assert` でコンパイルエラーとなるため、登録されていない型を誤って使用した場合にビルド時に検出できる。

特殊化は `SLUG_DEFINE_TYPE_RESOLVER` マクロで生成する（[7 章](#7-ユーザー型への対応) 参照）。

### 3.3 Variant と Instance の役割分担

| クラス | 所有権 | ヒープ確保 | 用途 |
|---|---|---|---|
| `Variant` | 値を所有 | あり | メソッド引数・戻り値・プロパティ値の受け渡し |
| `Instance` | 所有しない | なし | 既存オブジェクトへの参照（getter/setter の対象） |

`Instance` はポインタ 2 本（mutable / const）と `TypeId` だけを保持するため、コピーコストはゼロに近い。`Variant` はすべての値型に対してヒープ確保が発生するため、高頻度の呼び出しには注意が必要。

---

## 4. コンポーネント詳細

### 4.1 Type

型メタデータの本体。`ReflectionRegistry` に名前ハッシュをキーとして格納される。

| メンバ | 型 | 役割 |
|---|---|---|
| `m_id` | `TypeId` | 型名から生成したハッシュ |
| `m_name` | `core::String` | 型名文字列 |
| `m_base` | `const Type*` | 基底型（非所有・単一継承） |
| `m_properties` | `TVector<TReferencePtr<Property>>` | 登録済みプロパティ一覧 |
| `m_methods` | `TVector<TReferencePtr<Method>>` | 登録済みメソッド一覧 |
| `m_constructors` | `TVector<TReferencePtr<Constructor>>` | 登録済みコンストラクタ一覧 |

`IsDerivedFrom(other)` は `m_base` を辿るチェーン走査で継承関係を判定する。`FindProperty` / `FindMethod` は名前の線形探索（O(n)）。

### 4.2 Variant

型消去された値の所有コンテナ。`Concept` / `Model<T>` パターンで実装される。

```
Variant
  ├─ m_typeId : TypeId
  └─ m_ptr    : TReferencePtr<Concept>
                  └─ Model<T> : Concept
                       └─ value : T
```

- `Is<T>()` : `m_typeId` と `TypeOf<T>().GetId()` を比較
- `Get<T>()` : `static_cast` で直接キャスト（型チェックなし。必ず `Is<T>()` で確認してから呼ぶこと）
- デフォルト構築した `Variant` は `IsValid() == false`（null 相当）

### 4.3 Instance

既存オブジェクトへの非所有参照。型を消去した上で const / mutable を区別する。

```cpp
MyClass obj;
Instance inst(obj);           // mutable 参照
const MyClass cobj;
Instance cinst(cobj);         // const 参照（m_mutablePtr = nullptr）
```

`TryAs<T>()` は TypeId が一致した場合のみポインタを返す。const インスタンスに対して mutable 版 `TryAs<T>()` は常に `nullptr` を返す。

### 4.4 Property

プロパティのアクセサをラムダとして保持する。

```
Property
  ├─ m_name      : String
  ├─ m_ownerType : const Type*
  ├─ m_valueType : const Type*
  ├─ m_readable  : bool
  ├─ m_writable  : bool
  ├─ m_getter    : TFunctionObject<Variant(const Instance&)>
  └─ m_setter    : TFunctionObject<bool(const Instance&, const Variant&)>
```

読み取り専用プロパティは `m_setter = nullptr` / `m_writable = false` となる。

### 4.5 Method

メソッド呼び出しをラムダとして保持する。

```
Method
  ├─ m_name        : String
  ├─ m_ownerType   : const Type*
  ├─ m_returnType  : const Type*（void の場合 nullptr）
  ├─ m_paramTypes  : TVector<const Type*>
  ├─ m_isConst     : bool
  └─ m_invoke      : TFunctionObject<Variant(const Instance&, const TVector<Variant>&)>
```

const / 非 const メソッドはそれぞれ独立したオーバーロードで登録する。

### 4.6 Constructor

コンストラクタ呼び出しをラムダとして保持する。`Invoke` が返す `Variant` には値コピーされたオブジェクトが格納される。

### 4.7 ReflectionRegistry

プロセス内唯一のグローバルシングルトン。`TUnorderedMap<TypeId, Type>` で型を管理する。

```cpp
static ReflectionRegistry& GetRegistry();  // static local で初期化
Type& RegisterType<T>(StringView name);    // 既登録なら既存を返す
const Type* FindType(TypeId id) const;
const Type* FindType(const char* name) const;
```

---

## 5. 型登録フロー

型の登録は 2 ステップで行う。

**ステップ 1**: `TypeResolver` の特殊化を定義する。

```cpp
SLUG_DEFINE_TYPE_RESOLVER(MyClass, "MyClass")
```

このマクロは `TypeResolver<MyClass>::Get()` を生成し、`ReflectionRegistry` から `"MyClass"` の型を検索して返すようにする。

**ステップ 2**: `TypeRegister<T>` で型のメンバを登録する。

```cpp
Type& type = ReflectionRegistry::GetRegistry().RegisterType<MyClass>("MyClass");
TypeRegister<MyClass>(type)
    .base<BaseClass>()
    .constructor<>()
    .constructor<int, float>()
    .property("name",  &MyClass::m_name)
    .property("value", &MyClass::GetValue, &MyClass::SetValue)
    .property_readonly("id", &MyClass::GetId)
    .method("Update",  &MyClass::Update)
    .method("GetName", &MyClass::GetName);
```

`TypeRegister<T>` のメソッドはすべて `*this` を返すのでメソッドチェーンで記述できる。

内部では `std::index_sequence` を用いてパラメータパックを展開し、引数の取り出し（`VariantCast<Args>`）と関数呼び出しをラムダ化して格納する。

```
constructor<int, float>()  登録時:
  InvokeCtorImpl<MyClass, int, float> を index_sequence<0,1> で展開
  → lambda = [](args) { MyClass(args[0].Get<int>(), args[1].Get<float>()); }
  → ConstructorPtr に格納
```

---

## 6. 実行時利用フロー

### プロパティの読み書き

```
const Type* type = ReflectionRegistry::GetRegistry().FindType("MyClass");
auto prop = type->FindProperty("name");   // 線形探索

MyClass obj;
Instance inst(obj);

// 読み取り
Variant val = prop->GetValue(inst);
if (val.Is<core::String>())
{
    const core::String& name = val.Get<core::String>();
}

// 書き込み
prop->SetValue(inst, Variant(core::String("new name")));
```

```
GetValue(inst)
    │
    ▼
m_getter(inst)
    │
    ▼
inst.TryAs<MyClass>()  ─── TypeId 一致チェック
    │
    ▼
obj->*member  →  Variant(value)  ←── ヒープ確保
```

### メソッドの呼び出し

```
auto method = type->FindMethod("Update");
core::TVector<Variant> args = { Variant(1.0f) };
Variant result = method->Invoke(inst, args);
```

### 継承関係の確認

```
const Type* base = ReflectionRegistry::GetRegistry().FindType("BaseClass");
bool isDerived = type->IsDerivedFrom(base);  // m_base チェーン走査
```

---

## 7. ユーザー型への対応

### 最小構成

```cpp
// MyClass.hpp（または専用の登録ファイル）
#include "core/reflection/ReflectionUtility.hpp"

SLUG_DEFINE_TYPE_RESOLVER(MyClass, "MyClass")
```

```cpp
// 初期化時（エンジン起動時など）に一度呼ぶ
void RegisterMyClass()
{
    Type& type = ReflectionRegistry::GetRegistry().RegisterType<MyClass>("MyClass");
    TypeRegister<MyClass>(type)
        .property("value", &MyClass::m_value);
}
```

### getter/setter 方式

メンバ変数を直接公開したくない場合は getter / setter のメンバ関数ポインタを渡す。

```cpp
.property("value", &MyClass::GetValue, &MyClass::SetValue)
```

getter は `ReturnType (T::*)() const`、setter は `void (T::*)(ArgType)` の形式に限られる。getter の戻り値型と setter の引数型は（cv修飾除去後に）一致する必要があり、一致しない場合はコンパイルエラーとなる。

### 読み取り専用プロパティ

```cpp
.property_readonly("id", &MyClass::GetId)
```

setter を渡さず `m_writable = false` としたプロパティを登録する。`SetValue` を呼んでも `false` を返すだけで何も起きない。

---

## 8. 実装状況・既知の課題

| 機能 | 状態 |
|---|---|
| Type / Property / Method / Constructor の登録・検索 | 実装済み |
| Variant による型消去 | 実装済み |
| Instance による非所有参照 | 実装済み |
| 単一継承チェーン（`IsDerivedFrom`） | 実装済み |
| TypeRegister の Fluent Builder | 実装済み |
| PropertyDescriptor のエディタ連携 | 未実装（`Attribute.hpp` に定義あり、`Property` への統合なし） |
| ReflectionClass | 未実装（ヘッダ・cpp ともにほぼ空） |
| 多重継承 | 未対応 |
| プロパティ・メソッドの名前ハッシュ化（FindProperty/FindMethod の高速化） | 未対応（現状は線形探索） |

### 既知の問題

**Variant のアロケーションコスト**  
`Variant` はすべての値（`int` 1 個でも）に対して `MakeReference<Model<T>>` を呼ぶため、ヒープ確保が発生する。プロパティ・メソッドを頻繁に呼び出すパスではパフォーマンス上の懸念がある。Small Buffer Optimization (SBO) の導入が有効な対策となる。

**Get\<T\>() の型安全性**  
`Variant::Get<T>()` は型チェックを行わず `static_cast` で直接キャストする。`Is<T>()` での確認なしに呼び出した場合は未定義動作となる。デバッグビルドでのアサーション追加が望ましい。

**TypeRegister のテンプレートパラメータシャドウ**  
`TypeRegister<T>` の `RegisterType` メンバ関数が独自の `template<typename T>` を持ち、外側クラスの `T` をシャドウしている。このメンバ関数自体の実用性が低く、削除または名前変更が望ましい。
