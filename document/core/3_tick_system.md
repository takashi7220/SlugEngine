# Tick システム設計

## 目次

1. [概要](#1-概要)
2. [ディレクトリ構成](#2-ディレクトリ構成)
3. [設計の柱](#3-設計の柱)
4. [実行フロー](#4-実行フロー)
5. [依存ツリーによる実行順序制御](#5-依存ツリーによる実行順序制御)
6. [グループスケジューリング](#6-グループスケジューリング)
7. [自動クリーンアップ](#7-自動クリーンアップ)
8. [実装状況と既知の課題](#8-実装状況と既知の課題)

---

## 1. 概要

`core/tick` は、ゲームループの各フレームにおいてオブジェクトの更新処理（Tick）を管理するフレームワークである。

設計の根本方針は以下の 3 点。

- **ツリー構造による実行順序制御**: `TickFunction` が親子関係（依存ツリー）を形成し、親が先に実行されてから子が並列実行される。これにより、更新順序の依存関係を明示的に表現できる。
- **TaskSystem との統合による並列実行**: 兄弟関係にある `TickFunction`（同じ親の子）は `TaskSystem::ParallelFor` を通じて並列実行され、マルチコアを活用する。
- **参照カウントによる自動クリーンアップ**: 破棄されたオブジェクトの `TickFunction` は、次の実行タイミングで参照カウント検査により自動的に除去される。明示的な登録解除は不要。

---

## 2. ディレクトリ構成

```
tick/
├── TickFunction.hpp / TickFunction.cpp  # Tick実行単位と依存ツリーの管理
├── TickObject.hpp / TickObject.cpp      # Tickを受け取るオブジェクトの基底クラス
└── TickManager.hpp / TickManager.cpp    # グループ管理とフレーム単位の実行制御
```

---

## 3. 設計の柱

### 3.1 TickFunction — Tick 実行単位

`TickFunction` は `ReferenceObject` を継承し、`TReferencePtr<TickFunction>` で共有所有できる。

| メンバ | 役割 |
|---|---|
| `uuid` | 依存関係の登録・解除に使う一意識別子 |
| `type` | 所属グループ名（`FixedString<32>`） |
| `target` | Tick を呼び出す対象の `TickObject`（非所有ポインタ） |
| `parent` | 自分を dependents に持つ親 `TickFunction`（非所有ポインタ） |
| `dependents` | 自分の実行後に起動する子 `TickFunction` のリスト |
| `tickInterval` | Tick の最小実行間隔（秒）。0.0 で毎フレーム実行 |
| `preTickTime` | 直前の実行時刻。インターバル判定に使用 |

`Execute` の処理順は「自身の Tick → 子のクリーンアップ → 子の並列実行」。

### 3.2 TickObject — Tick を受け取る基底クラス

`Tick` を実装したいクラスは `TickObject` を継承する。コンストラクタで `m_primaryTickFunction` を生成し、`target` に自分自身を設定する。

`AddPrimaryTickDependency(other)` を呼ぶと、`other` の `m_primaryTickFunction` が自身の dependents に追加され、自分が先に Tick されてから `other` が Tick される関係が成立する。

### 3.3 TickGroup / TickManager — グループ管理とスケジューラ

`TickManager` は `IService` を継承し、`TaskSystem` に依存する（`SLUG_SERVICE_REGISTER_DEPENDENTS` による宣言）。

`TickGroup` はグループ名（`TickGroupType`）と、そのグループのルート `TickFunction` リストを保持する。ルートとは、`parent` を持たずに `TickManager` に直接登録された `TickFunction` のこと。

`Execute(param)` は `param.groupType` に一致するグループのルート関数のみを実行する。異なるグループは互いに独立しており、呼び出し側がグループを切り替えることで更新フェーズ（物理・ロジック・レンダリング等）を分離できる。

---

## 4. 実行フロー

```
TickManager::Execute(param)
        │
        ├─ TryGetGroup(param.groupType)  ─── 対象グループを取得
        │
        ├─ CleanFunction(group)          ─── 死亡オブジェクトと再ペアレント済みを除去
        │
        └─ ParallelFor(0, count, ...)    ─── ルート TickFunction を並列起動
                │
                ▼
        TickFunction::Execute(param)
                │
                ├─ CheckAndUpdateInterval  ── tickInterval が未経過なら target->Tick をスキップ
                ├─ target->Tick(param)     ── TickObject の Tick を呼び出す
                ├─ CleanDependents()       ── 参照カウント <= 1 の子を除去
                └─ ExecuteDependents(param)
                        │
                        └─ ParallelFor で dependents を再帰的に実行
```

---

## 5. 依存ツリーによる実行順序制御

`TickObject::AddPrimaryTickDependency` を使うと、オブジェクト間に「親が先、子が後」の実行順序を設定できる。

```
manager.Register(A)  // Aはルート
A.AddPrimaryTickDependency(&B)
A.AddPrimaryTickDependency(&C)
B.AddPrimaryTickDependency(&D)
```

上記構成での実行順序：

```
A                        （シングル）
├─ B ────────────────────┐
│    └─ D                │（B と C は並列、D は B の後）
└─ C ────────────────────┘
```

`TickFunction` は同時に 1 つの親しか持てない。`AddDependency` 時にすでに別の親がいれば、旧親から自動的に切り離される（リペアレント）。また、グループをまたいだ依存は `SLUG_ASSERT` で禁止されている。

---

## 6. グループスケジューリング

グループ名は `FixedString<32>` 型の `TickGroupType` で表される。組み込みグループは `SystemTickGroupType::Default` の 1 種類のみ。

ユーザーは任意の列挙値を `SLUG_NAMEOF` マクロで文字列化してグループ名として使用できる。

```cpp
// グループを定義
enum class MyTickGroup { Physics, Render };

// TickFunction に設定
obj.GetPrimaryTickFunction()->type = SLUG_NAMEOF(MyTickGroup::Physics);

// マネージャに登録
manager.Register(obj.GetPrimaryTickFunction());

// フレームごとにグループ指定で実行
param.groupType = SLUG_NAMEOF(MyTickGroup::Physics);
manager.Execute(param);
```

`Execute` は `param.groupType` に一致するグループ 1 つだけを処理する。複数グループを順序付きで実行したい場合は、呼び出し側が `Execute` を複数回呼ぶ。

---

## 7. 自動クリーンアップ

`TickObject` のデストラクタ時に `TickFunction` を明示的に解除する必要はない。クリーンアップは 2 箇所で行われる。

| タイミング | 処理 |
|---|---|
| `TickFunction::Execute` の直前 | `CleanDependents()` — 参照カウントが 1 以下（`TickFunction` 自身の `dependents` だけが参照）の子を削除 |
| `TickManager::Execute` の直前 | `CleanFunction(group)` — ルートリストの死亡関数を除去し、`parent` を持つ関数（再登録済み）を除外 |

`TickManager::CleanGroup()` は空になったグループ自体をリストから除去する（フレームごとには呼ばれず、呼び出し側が必要に応じて呼ぶ）。

---

## 8. 実装状況と既知の課題

| 機能 | 状態 |
|---|---|
| 依存ツリーによる実行順序制御 | 実装済み |
| グループ別フレーム実行 | 実装済み |
| 参照カウントによる自動クリーンアップ | 実装済み |
| `tickInterval` によるThrottling | 実装済み |
| 組み込みグループの種類 | `Default` の 1 種類のみ |
| テストコード | 全テストが `#if 0` で無効化されており未検証 |

`TickManager::TryGetGroup` はグループを値コピーで返しているため、`Execute` 内でコピーされたグループに対してクリーンアップを行っても元のリストに反映されない問題がある（`CleanFunction` の結果が破棄される）。将来的に参照返しまたは直接操作への変更が必要。
