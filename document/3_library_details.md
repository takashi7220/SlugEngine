# SlugEngine ライブラリ一覧

## 目次

1. [math](#1-math)
2. [core](#2-core)
3. [sgal](#3-sgal)
4. [image](#4-image)
5. [graphics/rhi](#5-graphicsrhi)
6. [graphics/shader](#6-graphicsshader)
7. [graphics/render](#7-graphicsrender)
8. [engine/engine_core](#8-engineengine_core)
9. [engine/engine_graphics](#9-engineengine_graphics)
10. [engine/engine_game](#10-engineengine_game)
11. [engine/engine_framework](#11-engineengine_framework)

---

## レイヤー構成

```
┌─────────────────────────────────────────────────────────────────┐
│  アプリケーション層    engine_framework                          │
├─────────────────────────────────────────────────────────────────┤
│  ゲームエンジン層      engine_core / engine_game / engine_graphics│
├─────────────────────────────────────────────────────────────────┤
│  グラフィックス層      render / rhi / shader                     │
├─────────────────────────────────────────────────────────────────┤
│  コア層               core / sgal / image                       │
├─────────────────────────────────────────────────────────────────┤
│  数学・基盤層          math                                      │
└─────────────────────────────────────────────────────────────────┘
```

---

## 1. math

**場所**: `libs/math/`  
**依存先**: なし

3D空間で必要な基本的な数学演算を提供する。ベクトル・行列・クォータニオン・トランスフォームなど、他のすべての層が利用する純粋な計算ライブラリ。エンジン固有の概念を持たない。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| ベクトル | `Vector2`, `Vector3`, `Vector4` |
| 行列 | `Matrix2x2`, `Matrix3x3`, `Matrix4x4`, `MatrixBase` |
| 回転 | `Quaternion` |
| 変換 | `Transform`（位置・回転・スケール） |
| 衝突 | `AABB`（軸並行境界ボックス） |
| ユーティリティ | `Math`（補間・三角関数等） |
| 型制約 | `TypeConcept`（算術型コンセプト） |

---

## 2. core

**場所**: `libs/core/`  
**依存先**: math

エンジン全体で共通して利用される基盤機能を提供する。特定のグラフィックス API やゲームの概念には依存せず、メモリ管理からシリアライズ・リフレクション・マルチスレッドまで広範な仕組みを担う。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| メモリ管理 | `MemoryObject`（全ヒープオブジェクトの基底）、`TReferencePtr` / `TWeakReferencePtr` / `TUniquePtr`（スマートポインタ）、mimalloc ベースのカスタムアロケータ |
| コンテナ | `TVector`, `TArray`, `TMap`, `TString`, `TSpan` など STL ラッパー |
| スレッド | `Thread`, `Mutex`, `ConditionVariable`, `TAtomic`, `Promise`, `Future` |
| タスク | `TaskSystem`, `Task`, `WorkQueue`（非同期タスクスケジューラ） |
| リフレクション | `Reflection`, `ReflectionClass`, `Property`, `Method`, `Variant`（ランタイム型情報） |
| シリアライズ | JSON / Binary 両対応のアーカイバ（`JsonInputArchive` / `BinaryOutputArchive` 等） |
| ファイルシステム | `FileSystem`, `FileMapping`, `IFileSystemController`（プラットフォーム抽象） |
| パックファイル | `PackFile`, `PackFilePool`（複数アセットを1ファイルにまとめる仕組み） |
| ウィンドウ | `IWindow`, `IWindowManager`（Windows実装と Null実装を提供） |
| サービスプロバイダ | `ServiceProvider`（DI コンテナ相当） |
| デバッグ | `Logger`, `Assert`, `MemoryCounter`（メモリリーク検出） |
| UUID | `UUID`, `NameID`（オブジェクト識別子） |
| タイマー / Tick | `Timer`, `TickManager`（フレーム進行管理） |
| カラー | `Color`, `Spectrum`, `ColorSpaceUtility` |
| パターン | `Singleton`, `ResourcePool`, `Closure`, `NotifyPropertyChanged` |

---

## 3. sgal

**場所**: `libs/sgal/`  
**依存先**: core

**slug graphics algorithm** の略。グラフィックスで利用される発展的なアルゴリズム、データ構造、算術を扱うライブラリ。空間分割・衝突判定・ジオメトリ表現など、レンダリングや物理処理の基盤となる構造を提供する。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| ジオメトリ | `Mesh`（メッシュ定義）、`Polygon`（ポリゴン）、`Vertex`（頂点定義） |
| 空間分割 | `Octree`, `OctreeFlat`（オクツリーによる空間分割） |
| 衝突判定 | `HitInterface`（ヒット結果の共通インターフェース） |

---

## 4. image

**場所**: `libs/image/`  
**依存先**: なし

画像データの読み込み・処理に特化した軽量ライブラリ。現時点では DDS フォーマット対応を主眼とする。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| フォーマット対応 | `DDS`（DirectDraw Surface 形式の解析・読み込み） |
| ユーティリティ | `ImageUtility`（画像処理共通ヘルパー） |

---

## 5. graphics/rhi

**場所**: `libs/graphics/rhi/`  
**依存先**: core

**Rendering Hardware Interface**。GPU へのアクセスを抽象化し、グラフィックス API の差異を隠蔽するハードウェア抽象化レイヤー。現在は Direct3D 12 の実装を提供する。上位層は `interface/` のみに依存することで、将来的な API 切り替えを可能にする。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| デバイス抽象 | `IDevice`, `IContext`, `ICommandList` |
| リソース抽象 | `IBuffer`, `ITexture`, `ISampler`, `IShader`, `IHeap` |
| パイプライン抽象 | `IGraphicsPipeline`, `IComputePipeline`, `IMeshletPipeline` |
| バインディング抽象 | `IBindingLayout`, `IBindingSet`, `IDescriptorTable` |
| レイトレーシング | `IAccelStruct`, `IOpacityMicromap`（BVH・OMM） |
| クエリ | `IEventQuery`, `ITimerQuery`（GPU 計測） |
| D3D12 実装 | 上記インターフェースの Direct3D 12 実装（39 クラス） |

---

## 6. graphics/shader

**場所**: `libs/graphics/shader/`  
**依存先**: なし

CPU 側コードと HLSL シェーダー間で共有する定数・定義を管理するライブラリ。シェーダーコードと C++ コードの定数を一元管理し、値の不一致を防ぐ。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| シェーダー定数 | `Constants.h`（バッファサイズ・スロット番号等） |
| 共通定義 | `Define.h`（マクロ・型エイリアス） |

---

## 7. graphics/render

**場所**: `libs/graphics/render/`  
**依存先**: core, rhi, shader

RHI の上に構築される高レベルレンダリングライブラリ。マテリアル・メッシュ・パイプラインを管理し、FrameGraph によるタスクベースのレンダリングを実現する。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| FrameGraph | `FrameGraph`（レンダリングパスの依存関係をグラフで記述・実行） |
| レンダリングパス | `IRenderPass`、`SurfacePass`、`PostprocessPass`、`ComputePass` |
| マテリアル | `Material`、`Shader`、`ShaderParameter`、`ConstantParameter` |
| ジオメトリ | `Mesh`、`Model`、`InputLayout`、`VertexAttribute` |
| パイプライン | `GraphicsPipeline`、`ComputePipeline`、`PipelineCache` |
| リソース管理 | `ResourceService` + 各種プール（Buffer, Texture, Shader, Sampler 等） |
| ドロー管理 | `DrawContext`、`DrawObject`、`IDrawObjectCuller`（カリング抽象） |
| テクスチャ | `Texture`、`Framebuffer` |
| ユーティリティ | `View`、`ScopedMarker`（GPU プロファイリングマーカー）、`PostProcessSettings` |

---

## 8. engine/engine_core

**場所**: `libs/engine/engine_core/`  
**依存先**: core

ゲームの「世界」を構成する概念を定義するライブラリ。アクター・コンポーネント・ワールド・アセット管理を担い、ゲームエンジン層の中核となる。グラフィックスの詳細を知らない。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| アクター | `Actor`（ゲームオブジェクト基底）、`ActorComponent`（コンポーネントシステム）、`SceneComponent`（シーン上の配置情報） |
| スポーン | `SpawnParameter`（アクター生成パラメータ） |
| ワールド | `IWorld`、`World`（シーン・空間の管理） |
| レベル | `Level`（ワールドを構成する単位） |
| アセット管理 | `AssetBase`、`AssetPool`、`AssetResolver`、`AssetService`（アセットのロード・参照管理） |
| ローダー | `LoaderService`、`LoadItem`（非同期ロード基盤） |

---

## 9. engine/engine_graphics

**場所**: `libs/engine/engine_graphics/`  
**依存先**: engine_core, render, image

グラフィックス機能をゲームエンジン層に統合するライブラリ。レンダリングに必要なアセット（テクスチャ・モデル・マテリアル・シェーダー）の管理と、アクターに付与するグラフィックスコンポーネントを提供する。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| グラフィックスアセット | `TextureAsset`、`ModelAsset`、`ShaderAsset`、`MaterialAsset` |
| アセットローダー | `GraphicsAssetLoader`（各グラフィックスアセット用ローダー基底） |
| メッシュコンポーネント | `MeshComponent`、`StaticMeshComponent`、`SkeltalMeshComponent` |
| プリミティブ | `PrimitiveComponent`（描画可能コンポーネントの基底） |
| パーサー | `TextureParser`、`ModelParser`、`MaterialParser`（ファイルからのアセット解析） |

---

## 10. engine/engine_game

**場所**: `libs/engine/engine_game/`  
**依存先**: engine_core

ゲーム固有のロジックを格納するライブラリ。現時点ではリフレクション登録（`ClassReflection.hpp`）を起点として、ゲームプレイ層の機能を追加していく場所として位置づけられる。

---

## 11. engine/engine_framework

**場所**: `libs/engine/engine_framework/`  
**依存先**: engine_graphics

エンジンのアプリケーション層を担うライブラリ。エンジン全体の初期化・メインループ・終了シーケンスを管理する。ユーザーはこの層を継承してアプリケーションを構築する。

### 提供機能

| カテゴリ | 内容 |
|---|---|
| フレームワーク | `Framework`（エンジン起動・ループ・終了の制御） |
| アプリケーション基底 | `ApplicationBase`（アプリケーション実装の継承元） |
