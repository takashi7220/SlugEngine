# Archiver シリアライズ設計

## 目次

1. [概要](#1-概要)
2. [ディレクトリ構成](#2-ディレクトリ構成)
3. [設計の柱](#3-設計の柱)
4. [呼び出しフロー](#4-呼び出しフロー)
5. [ユーザー型への対応](#5-ユーザー型への対応)
6. [実装状況](#6-実装状況)

---

## 1. 概要

`serialize/archiver` は、C++ オブジェクトをバイト列や構造化データへ変換（シリアライズ）し、元の状態に復元（デシリアライズ）するための汎用フレームワークである。

設計の根本方針は以下の 3 点。

- **コンパイル時解決**: ディスパッチを仮想関数ではなく CRTP と `if constexpr` で行い、実行時オーバーヘッドを排除する。
- **非侵襲的な拡張**: 既存の型を変更せずに `Serialize` / `Deserialize` 関数を追加するだけでシリアライズ対応できる。
- **フォーマット非依存**: ユーザーコードはアーカイブの種別（バイナリ、JSON 等）を意識しない。フォーマットを切り替えても型側のコードはそのまま使える。

---

## 2. ディレクトリ構成

```
serialize/
├── archiver/
│   ├── ArchiverSerializer.hpp       # 全機能の統合インクルード
│   ├── SerializeDispatch.hpp        # ディスパッチ関数
│   ├── archives/
│   │   ├── InputArchive.hpp         # 入力アーカイブ基底 (CRTP)
│   │   ├── OutputArchive.hpp        # 出力アーカイブ基底 (CRTP)
│   │   ├── binary/                  # バイナリフォーマット実装
│   │   └── json/                    # JSON フォーマット実装
│   └── types/
│       ├── SerializePrimitive.hpp   # bool / 整数 / 浮動小数点
│       ├── SerializeString.hpp      # core::String
│       └── SerializeVector.hpp      # core::TVector<T>
└── common/
    ├── SerializeTraits.hpp          # 関数存在判定（SFINAE）
    └── SerializeUtility.hpp         # ファイルヘッダー構造体
```

利用側は `ArchiverSerializer.hpp` を 1 つインクルードするだけでよい。

---

## 3. 設計の柱

### 3.1 CRTP による静的ポリモーフィズム

アーカイブはフォーマットごとに実装クラスを持つ（Binary, JSON 等）。共通インターフェース（`Value`, `Field`, `BeginArray` 等）は CRTP 基底クラスが提供し、フォーマット固有の処理（`WriteBool`, `WriteString` 等）は派生クラスが実装する。

仮想関数を使わないため、`ar.Value()` のような細粒度の呼び出しが大量に発生してもオーバーヘッドが蓄積しない。

バイナリフォーマットでは `BeginField` / `EndField` 等は無操作だが、JSON フォーマットでは `"key":` や `{}` といった構造の出力に使われる。同じユーザーコードでフォーマットを切り替えられる理由はここにある。

### 3.2 ディスパッチ機構

`ar.Value(obj)` が呼ばれると、`DispatchSerialize` が型 `T` に対して以下の優先順位でシリアライズ関数を選択する。

1. メンバー関数 `obj.Serialize(ar)` が存在する → それを呼ぶ
2. 非メンバー関数 `Serialize(ar, obj)` が存在する → それを呼ぶ
3. いずれも存在しない → コンパイルエラー

どの関数が使われるかは `SerializeTraits.hpp` の SFINAE 判定によってコンパイル時に確定する。プリミティブ・文字列・コンテナ等の組み込み型は `types/` 以下で非メンバー関数として定義されており、自動的にルーティングされる。

### 3.3 再帰的シリアライズ

コンテナ型（`TVector<T>` 等）の要素は `ar.Value()` で個別に処理される。`ar.Value()` は再び `DispatchSerialize` を呼ぶため、要素型がユーザー型・文字列・ネストされたコンテナであっても同一のルーティングに乗る。

---

## 4. 呼び出しフロー

```
ar.Value(obj) / ar.Field("name", obj)
        │
        ▼
  DispatchSerialize ── 型判定（コンパイル時）
    ├─ obj.Serialize(ar) が存在        → メンバー関数呼び出し
    └─ Serialize(ar, obj) が存在       → 非メンバー関数呼び出し
        │
        ▼
  ar.WriteBool() / WriteIntenger() / WriteString() 等
        │
        ▼
  フォーマット実装（Binary / JSON / ...）
```

デシリアライズも同じ構造で `Deserialize` 関数・`Read*` メソッドを辿る。

---

## 5. ユーザー型への対応

### メンバー関数方式

`private` メンバーに直接アクセスしたい場合に適している。`Serialize`（出力）と `Deserialize`（入力）をそれぞれ定義する。

```cpp
template<typename Archive>
void Serialize(Archive& ar) const
{
    ar.Field("name", m_name);
    ar.Field("value", m_value);
}

template<typename Archive>
void Deserialize(Archive& ar)
{
    ar.Field("name", m_name);
    ar.Field("value", m_value);
}
```

### 非メンバー関数方式

型を変更できない場合、またはシリアライズロジックを型と分離したい場合に適している。型と同じ namespace に定義することで ADL により発見される。

### 継承がある場合

基底クラス部分は `ar.BaseClass(BaseClassWrapper<Base>(*this))` で明示的に処理する。派生クラスの `Serialize` の先頭で呼ぶことで、基底→派生の順にシリアライズされる。

---

## 6. 実装状況

| アーカイブ | 状態 |
|---|---|
| Binary（入出力） | 実装済み |
| JSON（入出力） | インターフェースのみ・未実装 |

バイナリフォーマットはエンディアン変換未対応。また、バッファオーバーラン検出に `SLUG_THROW_EXCEPTION` を使用しているが、プロジェクトは例外不使用方針のため将来的にアサーション or エラー戻り値への変更が必要。
