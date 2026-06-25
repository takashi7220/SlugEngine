# RHI (Rendering Hardware Interface) 設計解説

## 目次

1. [概要](#1-概要)
2. [ディレクトリ構成](#2-ディレクトリ構成)
3. [レイヤー構造](#3-レイヤー構造)
4. [主要インターフェース](#4-主要インターフェース)
5. [リソース型定義](#5-リソース型定義)
6. [バインディングシステム](#6-バインディングシステム)
7. [パイプライン設計](#7-パイプライン設計)
8. [状態管理システム](#8-状態管理システム)
9. [メモリ管理](#9-メモリ管理)
10. [レイトレーシング対応](#10-レイトレーシング対応)
11. [D3D12実装層](#11-d3d12実装層)
12. [主要定数](#12-主要定数)

---

## 1. 概要

RHI (Rendering Hardware Interface) は、GPU APIの差異を吸収するグラフィックス抽象化層である。
上位のグラフィックスエンジン層はプラットフォーム固有のAPIを直接扱わず、RHIのインターフェースのみを使用する。

現時点のバックエンド実装は **Direct3D 12** のみ。設計上は Vulkan や D3D11 への移植を考慮した構造になっている。

対応機能:
- グラフィクス / コンピュート / コピーの3キュー
- メッシュレットパイプライン (Amplification + Mesh Shader)
- レイトレーシング (DXR / BVH, OMM)
- Variable Rate Shading (VRS)
- バインドレスリソース

---

## 2. ディレクトリ構成

```
libs/graphics/rhi/
├── include/rhi/
│   ├── Rhi.hpp                  # エントリヘッダ
│   ├── ResourceType.hpp         # 全型定義・列挙型・構造体 (約900行)
│   ├── common/                  # プラットフォーム共通ユーティリティ
│   │   ├── BitsetAllocator.hpp
│   │   ├── Resource.hpp         # IResource 基底
│   │   ├── StateTracking.hpp    # リソース状態追跡
│   │   └── Utility.hpp
│   ├── interface/               # 抽象インターフェース (I*.hpp)
│   │   ├── IDevice.hpp
│   │   ├── ICommandList.hpp
│   │   ├── IBuffer.hpp
│   │   ├── ITexture.hpp
│   │   ├── IShader.hpp
│   │   ├── IGraphicsPipeline.hpp
│   │   ├── IComputePipiline.hpp
│   │   ├── IMeshletPipeline.hpp
│   │   ├── IBindingLayout.hpp
│   │   ├── IBindingSet.hpp
│   │   ├── IDescriptorTable.hpp
│   │   ├── IFrameBuffer.hpp
│   │   ├── IHeap.hpp
│   │   ├── ISampler.hpp
│   │   ├── IInputLayout.hpp
│   │   ├── IEventQuery.hpp
│   │   ├── ITimerQuery.hpp
│   │   ├── IMessageCallback.hpp
│   │   └── rt/                  # レイトレーシング専用
│   │       ├── IAccelStruct.hpp
│   │       ├── IOpacityMicromap.hpp
│   │       └── IPipeline.hpp
│   └── d3d12/                   # D3D12固有の型・実装ヘッダ
│       ├── interface/           # D3D12拡張インターフェース
│       └── resource/            # D3D12リソース実装クラス
└── src/
    ├── common/                  # 共通実装
    │   ├── FormatInfo.cpp
    │   ├── StateTracking.cpp
    │   └── Utility.cpp
    └── d3d12/                   # D3D12実装
        ├── resource/
        │   ├── Device.cpp       # 約2,075行
        │   └── CommandList.cpp  # 約2,533行 (最大)
        └── ...
```

---

## 3. レイヤー構造

```
上位グラフィックスエンジン
        ↓ (インターフェースのみ使用)
┌──────────────────────────────────┐
│  抽象インターフェース層            │
│  interface/ の I*.hpp            │
│  IDevice, ICommandList, ...      │
└──────────────────────────────────┘
        ↓ (継承・実装)
┌──────────────────────────────────┐
│  D3D12 実装層                    │
│  d3d12/resource/ の *.hpp/.cpp   │
│  Device, CommandList, ...        │
└──────────────────────────────────┘
        ↓
   Direct3D 12 API
```

全リソースクラスは `IResource` を継承し、参照カウント (`AddRef` / `Release`) で寿命管理される。
上位層は `TReferencePtr<IXxx>` でリソースを保持する。

---

## 4. 主要インターフェース

### IDevice

RHIの中核。リソースの作成とコマンドの実行を担う。

主な責務:
- リソース作成: `CreateTexture`, `CreateBuffer`, `CreateShader`, `CreateSampler`
- パイプライン作成: `CreateGraphicsPipeline`, `CreateComputePipeline`, `CreateMeshletPipeline`
- バインディング作成: `CreateBindingLayout`, `CreateBindingSet`
- ヒープ作成・メモリバインド: `CreateHeap`, `BindTextureMemory`
- コマンドリスト実行: `ExecuteCommandLists`
- フィーチャクエリ: `QueryFeatureSupport`

### ICommandList

GPU命令の記録と送信を担う。

主な責務:
- 状態設定: `SetGraphicsState`, `SetComputeState`, `SetMeshletState`
- 描画コマンド: `Draw`, `DrawIndexed`, `DrawIndirect`
- コンピュート: `Dispatch`, `DispatchIndirect`
- メッシュレット: `DispatchMesh`
- レイトレーシング: `DispatchRays`, `BuildBottomLevelAccelStruct`, `BuildTopLevelAccelStruct`
- リソース操作: `ClearTexture`, `ClearBuffer`, `WriteBuffer`, `CopyTexture`
- バリア: `SetTextureState`, `SetBufferState`, `CommitBarriers`

### IContext

デバイス作成のエントリポイント。アダプタ列挙とデバイスの初期化を行う。

---

## 5. リソース型定義

`ResourceType.hpp` に全型が定義されている。

### FormatType

テクスチャ・バッファのデータフォーマット。

```
R8_UNORM, RG8_UNORM, RGBA8_UNORM, ...
R16_FLOAT, RGBA16_FLOAT, ...
R32_FLOAT, RGBA32_FLOAT, ...
D16, D24S8, D32, D32S8  (深度フォーマット)
BC1_UNORM 〜 BC7_UNORM  (圧縮フォーマット)
```

### ResourceType

バインディング時のビューの種別。

| 種別 | 説明 |
|---|---|
| `Texture_SRV` | シェーダーリソースビュー |
| `Texture_UAV` | アンオーダードアクセスビュー |
| `TypedBuffer_SRV/UAV` | 型付きバッファ |
| `StructuredBuffer_SRV/UAV` | 構造化バッファ |
| `RawBuffer_SRV/UAV` | 生バイトバッファ |
| `ConstantBuffer` | 定数バッファ |
| `VolatileConstantBuffer` | フレーム毎に更新される定数バッファ |
| `Sampler` | サンプラー |
| `RayTracingAccelStruct` | レイトレーシング加速構造 |
| `PushConstants` | プッシュ定数 |

### ResourceStates

リソースの現在の使われ方を表すビットフラグ。

主な状態: `ConstantBuffer`, `VertexBuffer`, `IndexBuffer`, `ShaderResource`, `UnorderedAccess`, `RenderTarget`, `DepthWrite`, `DepthRead`, `CopyDest`, `CopySource`, `Present`, `AccelStructRead`, `AccelStructWrite`

---

## 6. バインディングシステム

シェーダーへのリソース供給は **BindingLayout → BindingSet** の2段階で定義する。

### BindingLayout (レイアウト定義)

どのスロットにどの型のリソースが入るかを宣言する。パイプライン作成時に指定する。

```cpp
BindingLayoutDesc desc;
desc.visibility = ShaderType::All;
desc.registerSpace = 0;
desc.bindings = {
    BindingLayoutItem::Create(0, ResourceType::ConstantBuffer),
    BindingLayoutItem::Create(1, ResourceType::Texture_SRV),
    BindingLayoutItem::Create(0, ResourceType::Sampler),
    BindingLayoutItem::PushConstants(2, sizeof(MyConstants)),
};
```

### BindingSet (実リソースの割り当て)

LayoutのスロットにIResource実体を差し込む。描画前に差し替えることができる。

```cpp
BindingSetDesc desc;
desc.bindings = {
    BindingSetItem::ConstantBuffer(0, myBuffer),
    BindingSetItem::Texture_SRV(1, myTexture, TextureSubresourceSet{}),
    BindingSetItem::Sampler(0, mySampler),
};
```

### DescriptorTable (動的バインドレス)

`IDescriptorTable` は `IBindingSet` を継承し、実行時にサイズを変更できる動的なバインドレスバインディングを提供する。
`ResizeDescriptorTable` / `WriteDescriptorTable` でインデックスに個別書き込みが可能。

最大5つのBindingLayout / BindingSetをパイプラインに同時に束ねることができる (`MAX_BINDING_LAYOUTS = 5`)。

---

## 7. パイプライン設計

### GraphicsPipelineDesc

```
シェーダー: VS, HS, DS, GS, PS
入力レイアウト: IInputLayout
プリミティブ型: Triangle, Line, etc.
レンダーステート:
  ├─ BlendState      (ブレンドモード、最大8 RenderTarget)
  ├─ DepthStencilState
  └─ RasterState     (塗りつぶし、カリング)
BindingLayouts
```

### ComputePipelineDesc

```
シェーダー: CS のみ
BindingLayouts
```

### MeshletPipelineDesc

```
シェーダー: AS (Amplification), MS (Mesh), PS (Pixel)
レンダーステート (Graphicsと同等)
BindingLayouts
```

---

## 8. 状態管理システム

### 自動バリア管理

`SetEnableAutomaticBarriers(true)` を設定すると、`SetTextureState` / `SetBufferState` の呼び出しから必要なリソースバリアを自動生成する。

状態追跡は `CommandListResourceStateTracker` が担い、バッファ・テクスチャごとに現在の `ResourceStates` を記録する。テクスチャはサブリソース (MIPレベル × 配列スライス) 単位で追跡される。

### バージョニング (フレーム同期)

コマンドリストの実行状態はバージョン番号で管理される。

```
uint64_t バージョン = ID (60bit) | Queue (3bit) | SubmittedFlag (1bit)
```

`QueueWaitForCommandList(waitQueue, sourceQueue, version)` でキュー間の依存を宣言できる。

---

## 9. メモリ管理

### Heap (明示的なVRAM確保)

```cpp
HeapDesc desc;
desc.capacity = 64 * 1024 * 1024;
desc.type = HeapType::DeviceLocal;
auto heap = device->CreateHeap(desc);

// MemoryRequirements を取得してから Heap へバインド
auto req = device->GetTextureMemoryRequirements(texture);
device->BindTextureMemory(texture, heap, offset);
```

### VolatileConstantBuffer

`BufferDesc::maxVersions` を設定することで、同一バッファのN世代を内部に保持できる。
`ICommandList::WriteBuffer` で書き込むと、コマンドリスト実行中に上書きされない。
CommandList内部の `UploadManager` がフレーム内アップロード用のバッファチャンクを管理する。

---

## 10. レイトレーシング対応

`namespace rhi::rt` にレイトレーシング固有のインターフェースが集まっている。

### 加速構造 (IAccelStruct)

- **BLAS (Bottom Level)**: ジオメトリ (三角形 / AABB) ごとに構築
- **TLAS (Top Level)**: BLASインスタンスの一覧から構築

```cpp
// BLASの記述
AccelStructDesc desc;
desc.isTopLevel = false;
desc.bottomLevelGeometries = {
    GeometryDesc::Triangles(indexBuffer, vertexBuffer, ...)
};
```

Opacity Micromap (OMM) による高精度な不透明度情報も `IOpacityMicromap` で対応している。

### レイトレーシングパイプライン (rt::IPipeline)

```
Shaders: RayGen, Miss, Callable
HitGroups: ClosestHit, AnyHit, Intersection
maxPayloadSize, maxRecursionDepth
```

シェーダーテーブル (`IShaderTable`) にシェーダーレコードを書き込み、`DispatchRays` で実行する。

---

## 11. D3D12実装層

`libs/graphics/rhi/include/rhi/d3d12/` および `src/d3d12/` に D3D12 固有の実装がある。

### 主要クラス

| クラス | 役割 |
|---|---|
| `d3d12::Device` | `IDevice` 実装。キュー・DescriptorHeap・DeviceResources を保持 |
| `d3d12::CommandList` | `ICommandList` 実装。UploadManager・StateTracker を保持 |
| `d3d12::RootSignature` | バインディングレイアウトからD3D12のRootSignatureを生成 |
| `d3d12::DescriptorHeap` | SRV / RTV / DSV / Sampler の4ヒープを管理 |
| `d3d12::StaticDescriptorHeap` | 固定サイズのディスクリプタを管理 |
| `d3d12::UploadManager` | フレーム内アップロード用チャンクバッファ管理 |

### D3D12固有インターフェース

`d3d12::IDevice` / `d3d12::ICommandList` は抽象インターフェースを継承しつつ、D3D12固有のAPIを追加で公開する。上位層は `rhi::IDevice` を介して操作し、D3D12層を直接参照しない。

---

## 12. 主要定数

| 定数 | 値 | 説明 |
|---|---|---|
| `MAX_RENDER_TARGETS` | 8 | 同時レンダーターゲット数 |
| `MAX_VIEWPORTS` | 16 | 同時ビューポート数 |
| `MAX_VERTEX_ATTRIBUTES` | 16 | 頂点属性数 |
| `MAX_BINDING_LAYOUTS` | 5 | パイプラインに束ねるレイアウト数 |
| `MAX_BINDING_PER_LAYOUT` | 128 | 1レイアウトのバインディング数 |
| `MAX_VOLATILE_CONSTANT_BUFFERS` | 32 | ボラタイルCBの総数 |
| `MAX_PUSH_CONSTANT_SIZE` | 128 (bytes) | プッシュ定数の最大サイズ |
| `CONSTANT_BUFFER_OFFSET_SIZE_ALIGNMENT` | 256 (bytes) | CBVオフセットのアライメント |
