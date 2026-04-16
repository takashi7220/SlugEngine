# SlugEngine — Claude向けプロジェクト情報

詳細なコーディング規約は [document/1_coding_style.md](document/1_coding_style.md)、設計思想は [document/2_project_design.md](document/2_project_design.md) を参照。以下はその要約。

---

## コーディング規約

### 基本
- C++20 / MSVC (Windows) プライマリ
- STLは `slug::core` の型エイリアスを通じて使用
- 例外不使用（アサーション or 戻り値でエラー通知）
- RAII徹底

### 命名規則

| 対象 | スタイル | 例 |
|---|---|---|
| クラス・構造体・列挙型 | PascalCase | `ReferenceObject` |
| テンプレートクラス・型エイリアス | `T` プリフィックス + PascalCase | `TReferencePtr<T>`, `TVector<T>` |
| 非テンプレート型エイリアス | PascalCase（T なし） | `String`, `ObjectType` |
| 関数・メソッド | PascalCase | `AddRef()`, `MakeReference()` |
| STL互換 getter | lowercase | `get()`, `reset()`, `swap()` |
| メンバ変数 | `m_` + camelCase | `m_ptr`, `m_control` |
| 値型 struct の公開フィールド | snake_case | `strong`, `weak` |
| namespace | snake_case | `slug::core` |
| Concept | PascalCase | `HasAddRefAndRelease` |
| マクロ | `SLUG_` + UPPER_SNAKE_CASE | `SLUG_NODISCARD` |
| インターフェース | `I` プリフィックス | `ILogCategory` |
| 列挙値 | PascalCase | `Info`, `Warning` |

### namespace
- C++17 スコープ付き namespace を使用（ネスト形式不可）: `namespace slug::core { }`
- スコープ内はインデントしない
- 閉じ括弧コメントは付けない

### クラス設計
- アクセス修飾子順: `public` → `protected` → `private`
- コピー・ムーブは `= delete` or `= default` を明示
- ヒープ確保されるすべてのオブジェクトは `MemoryObject` を継承

### フォーマット
- インデント: 4スペース（タブ不使用）
- 波括弧: Allman スタイル（次の行）
- 目安 120 文字/行
- メンバ初期化子リストは各メンバ別行
- `#pragma once` のみ使用（`#ifndef` ガード不使用）

### 修飾子
- 例外を投げない関数には必ず `noexcept`
- 読み取り専用メンバ関数には必ず `const`
- オーバーライドには必ず `override`
- 戻り値必須の関数には `SLUG_NODISCARD`

### メモリ管理
- `TReferencePtr<T>` — 共有所有
- `TWeakReferencePtr<T>` — 弱参照
- `TUniquePtr<T>` — 単独所有
- 生ポインタで所有権を持たない。`MakeReference` / `MakeUnique` でオブジェクト生成

---

## プロジェクト設計

### レイヤー構成（上→下）
1. アプリケーション層 — エントリポイント・実行ループ
2. ゲームエンジン層 — アクタ・アセット・ワールド
3. グラフィックス層 — レンダリング・RHI
4. コア層 — メモリ・コンテナ・リフレクション・シリアライズ
5. 数学・基盤層 — ベクトル・行列・アロケータ

上位層は下位層を知っているが、下位層は上位層を知らない。層を飛び越えた依存は持たない。

### フォルダ構成
- `libs/<名前>/include/<名前>/` — 公開ヘッダ
- `libs/<名前>/src/` — 非公開実装
- `thirdparty/` — 外部ライブラリ（直接編集禁止）
- `tools/` — コード生成ツール（ランタイム非含有）
- `_workspace/` — ビルド成果物（ソースツリー外）

### 設計原則
- 明示性・交換可能性・検証可能性を優先
- 新機能追加時は既存層の拡張より新層の追加を優先
- 循環依存は設計ミスのサイン
- 交換可能なものへの依存は `I` プリフィックスの抽象インターフェースを介する
