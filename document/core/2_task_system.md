# タスクシステム設計

## 目次

1. [概要](#1-概要)
2. [ディレクトリ構成](#2-ディレクトリ構成)
3. [設計の柱](#3-設計の柱)
4. [タスク実行フロー](#4-タスク実行フロー)
5. [依存関係の解決](#5-依存関係の解決)
6. [ParallelFor の仕組み](#6-parallelfor-の仕組み)
7. [シャットダウン手順](#7-シャットダウン手順)
8. [実装状況](#8-実装状況)

---

## 1. 概要

`core/task` は、複数のワーカースレッドにタスクを分配して並列実行するスレッドプールシステムである。

設計の根本方針は以下の 3 点。

- **依存グラフによるスケジューリング**: タスク間の前後関係を `pending` カウンタで管理し、前提タスクがすべて完了した時点で自動的にキューへ投入する。
- **Future/Promise による完了通知**: タスクの完了は `TFuture<void>` を通じて待機でき、呼び出し元がポーリングする必要はない。
- **サービス統合**: `TaskSystem` は `IService` を継承しており、`ServiceProvider` 経由でエンジン全体から取得できる。

---

## 2. ディレクトリ構成

```
task/
├── Task.hpp / Task.cpp           # タスク単体の定義と実行ロジック
├── TaskSystem.hpp / TaskSystem.cpp  # スレッドプールとスケジューラ
└── WorkQueue.hpp / WorkQueue.cpp    # スレッドセーフなタスクキュー
```

利用側は `TaskSystem.hpp` を 1 つインクルードすれば、`Task.hpp` と `WorkQueue.hpp` も同時に取り込まれる。

---

## 3. 設計の柱

### 3.1 Task — 実行単位

`Task` は `ReferenceObject` を継承するため、`TReferencePtr<Task>` で共有所有できる。

| メンバ | 役割 |
|---|---|
| `func` | 実行する処理本体（`TFunctionObject<void()>`） |
| `done` | 完了通知用 `TPromise<void>` |
| `pending` | 実行を保留している前提タスクの残数（アトミック） |
| `started` | 二重起動を防ぐフラグ（アトミック） |
| `finished` | 完了フラグ（アトミック） |
| `dependents` | このタスク完了後に実行を試みる依存タスクリスト |

### 3.2 WorkQueue — スレッドセーフキュー

内部に `TQueue<TReferencePtr<Task>>` と `ConditionVariable` を持ち、3 種類のアクセスを提供する。

| 関数 | 動作 |
|---|---|
| `Push` | タスクを末尾に追加し、待機中の 1 スレッドを起こす |
| `TryPop` | キューが空なら即時 `false` を返す（ノンブロッキング） |
| `PopBlocking` | キューが空かつストップフラグが `false` の間、スレッドをブロックする |

### 3.3 TaskSystem — スケジューラ

`IService` を継承するスレッドプール。`Desc::workerCount` が 0 の場合は `hardware_concurrency()` を自動採用する。

ワーカースレッドは `WorkerMain()` の無限ループ内で `PopBlocking` を呼び続け、キューに投入されたタスクを逐次実行する。ストップフラグが立ち、かつキューが空になるとループを抜けてスレッドが終了する。

---

## 4. タスク実行フロー

```
TaskSystem::Launch(func, prerequisites)
        │
        ▼
  新しい Task を生成、pending = 前提タスク数
        │
  前提タスクが 0 (または全完了済み)
        │
        ▼
  TaskSystem::Enqueue ─── m_inflight をインクリメント
        │
        ▼
  WorkQueue::Push ─── ConditionVariable で待機中のワーカーを起こす
        │
        ▼
  WorkerMain: PopBlocking → task->Run(*this)
        │
        ▼
  Task::Run
    1. started フラグを CAS でセット（二重起動ガード）
    2. func() を実行
    3. done.set_value() で Future を解決
    4. finished = true
    5. dependents の pending をデクリメント → 0 になったものを Enqueue
    6. TaskSystem::OnTaskFinished() で m_inflight をデクリメント
```

---

## 5. 依存関係の解決

`Launch` に `prerequisites` を渡すと、各前提タスクの `dependents` リストに新タスクを登録し、`pending` を前提数に設定する。

```
TaskA ──┐
        ├─→ TaskC (pending=2)
TaskB ──┘
```

`TaskA` が完了すると `TaskC->pending` が 2→1 に、`TaskB` が完了すると 1→0 になり、その時点で `TaskC` がキューに投入される。

前提タスクがすでに完了済みの場合は `TryAddDependent` が `false` を返すため、`pending` をカウントせずに処理する。これにより、完了済みタスクを前提として渡しても安全に動作する。

---

## 6. ParallelFor の仕組み

`ParallelFor(begin, end, options, body)` は指定範囲を複数チャンクに分割し、各チャンクを独立したタスクとして並列実行する。

```
grain = (options.grain == 0) ? workerCount × GRAIN_BIAS_RATE : options.grain
numChunks = ceil((end - begin) / grain)

チャンクタスク [0..numChunks-1]
    それぞれ body(i) を grain 個ずつ実行
    ↓ 完了時
joinTask の pending をデクリメント
    ↓ 全チャンク完了
joinTask が Enqueue → Future が解決
```

`GRAIN_BIAS_RATE = 4` により、デフォルトのチャンク数はワーカー数の 4 倍となる。チャンクが多めになることで、処理時間のばらつきによるワーカーの遊びを減らす（ロードバランシング）。

`ParallelForAsync` は `TFuture<void>` を返して即時復帰する。`ParallelFor` はその Future を `get()` で待つ同期版ラッパーである。

---

## 7. シャットダウン手順

`Terminate()` は以下の順序で安全に停止する。

1. `m_stopFlag` を CAS で `true` にセット（二重呼び出しガード）
2. `WorkQueue::NotifyAll()` で待機中の全ワーカーを起こす
3. `m_shutdownCv.wait` で `m_inflight == 0` になるまでブロック
4. 全ワーカースレッドを `join` して破棄

`OnTaskFinished()` が `m_inflight` を 0 にした瞬間に `m_shutdownCv.notify_all()` が呼ばれるため、`Terminate()` の待機が解除される。

---

## 8. 実装状況

| 機能 | 状態 |
|---|---|
| スレッドプール（ワーカー自動起動） | 実装済み |
| 依存グラフによるタスクスケジューリング | 実装済み |
| `ParallelFor` / `ParallelForAsync` | 実装済み |
| タスク優先度 | 未実装 |
| タスクのキャンセル | 未実装 |

`Task::Run` 内では `try/catch` で例外をキャッチして `done.set_exception()` に渡しているが、プロジェクトは例外不使用方針のため将来的にエラー戻り値ベースへの変更が必要。
