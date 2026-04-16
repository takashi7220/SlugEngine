#include "core/memory/UniquePtr.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "rhi/common/Resource.hpp"
#include "rhi/common/Utility.hpp"
#include "rhi/d3d12/D3D12Utility.hpp"
#include "rhi/d3d12/D3D12Raytracing.hpp"
#include "rhi/d3d12/resource/Device.hpp"
#include "rhi/d3d12/resource/Heap.hpp"
#include "rhi/d3d12/resource/Texture.hpp"
#include "rhi/d3d12/resource/Sampler.hpp"
#include "rhi/d3d12/resource/Shader.hpp"
#include "rhi/d3d12/resource/InputLayout.hpp"
#include "rhi/d3d12/resource/EventQuery.hpp"
#include "rhi/d3d12/resource/GraphicsPipeline.hpp"
#include "rhi/d3d12/resource/ComputePipeline.hpp"
#include "rhi/d3d12/resource/MeshletPipeline.hpp"
#include "rhi/d3d12/resource/BindingLayout.hpp"
#include "rhi/d3d12/resource/BindingSet.hpp"
#include "rhi/d3d12/resource/BindlessLayout.hpp"
#include "rhi/d3d12/resource/DescriptorTable.hpp"
#include "rhi/d3d12/resource/AccelStruct.hpp"

constexpr uint32_t MAX_COMMANDLIST_TO_EXECUTE = 64;

namespace slug::rhi::d3d12
{

rhi::DevicePtr Device::Create(const DeviceDesc& desc)
{
    Device* device = new Device(desc);
    return DevicePtr::create(device);
}

Device::Device(const DeviceDesc& desc)
    : m_resources(m_context, desc)
{
    m_context.device = desc.pDevice;

    if (desc.pGraphicsCommandQueue)
    {
        m_queues[int32_t(CommandQueue::Graphics)] = core::MakeUnique<Queue>(m_context, desc.pGraphicsCommandQueue);
    }
    if (desc.pComputeCommandQueue)
    {
        m_queues[int(CommandQueue::Compute)] = core::MakeUnique<Queue>(m_context, desc.pComputeCommandQueue);
    }
    if (desc.pComputeCommandQueue)
    {
        m_queues[int(CommandQueue::Copy)] = core::MakeUnique<Queue>(m_context, desc.pCopyCommandQueue);
    }

    // 各種静的なリソースをアロケートしておく。
    m_resources.depthStencilViewHeap.AllocateResources(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, desc.depthStencilViewHeapSize, false);
    m_resources.renderTargetViewHeap.AllocateResources(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, desc.renderTargetViewHeapSize, false);
    m_resources.shaderResourceViewHeap.AllocateResources(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, desc.shaderResourceViewHeapSize, true);
    m_resources.samplerHeap.AllocateResources(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, desc.samplerHeapSize, true);

    // サポートしているオプションを調査
    m_context.device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &m_options, sizeof(m_options));
    bool hasOptions5 = SUCCEEDED(m_context.device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &m_options5, sizeof(m_options5)));
    bool hasOptions6 = SUCCEEDED(m_context.device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &m_options6, sizeof(m_options6)));
    bool hasOptions7 = SUCCEEDED(m_context.device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &m_options7, sizeof(m_options7)));

    if (SUCCEEDED(m_context.device->QueryInterface(&m_context.device5)) && hasOptions5)
    {
        m_rayTracingSupported = m_options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0;
        m_traceRayInlineSupported = m_options5.RaytracingTier >= D3D12_RAYTRACING_TIER_1_1;
    }

    // メッシュレットに対応しているか判定
    if (SUCCEEDED(m_context.device->QueryInterface(&m_context.device2)) && hasOptions7)
    {
        m_meshletsSupported = m_options7.MeshShaderTier >= D3D12_MESH_SHADER_TIER_1;
    }

    // VRSに対応しているか判定
    if (hasOptions6)
    {
        m_variableRateShadingSupported = m_options6.VariableShadingRateTier >= D3D12_VARIABLE_SHADING_RATE_TIER_2;
    }

    // ルートシグニチャを生成
    {
        D3D12_INDIRECT_ARGUMENT_DESC argDesc = {};
        D3D12_COMMAND_SIGNATURE_DESC csDesc = {};
        csDesc.NumArgumentDescs = 1;
        csDesc.pArgumentDescs = &argDesc;

        csDesc.ByteStride = 16;
        argDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
        m_context.device->CreateCommandSignature(&csDesc, nullptr, IID_PPV_ARGS(&m_context.drawIndirectSignature));

        csDesc.ByteStride = 20;
        argDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
        m_context.device->CreateCommandSignature(&csDesc, nullptr, IID_PPV_ARGS(&m_context.drawIndexedIndirectSignature));

        csDesc.ByteStride = 12;
        argDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
        m_context.device->CreateCommandSignature(&csDesc, nullptr, IID_PPV_ARGS(&m_context.dispatchIndirectSignature));
    }

    // フェンスを生成
    m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);

    // 実行するコマンドリストをリザーブ
    m_commandListsToExecute.reserve(MAX_COMMANDLIST_TO_EXECUTE);
}

Device::~Device()
{
    WaitForIdle();

    if (m_fenceEvent)
    {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }
}

Context& Device::GetContext()
{
    return m_context;
}

Object Device::GetNativeObject(ObjectType objectType)
{
    switch (objectType)
    {
    case object_types::D3D12_Device:
        return Object(m_context.device);
    case object_types::rhi_D3D12_Device:
        return Object(this);
    default:
        return nullptr;
    }
}

GraphicsApiType Device::GetGraphicsAPI()
{
    return GraphicsApiType::D3D12;
}


bool Device::GetNvapiIsInitialized() const
{
    return m_nvapiIsInitialized;
}

Queue* Device::GetQueue(CommandQueue type)
{
    return m_queues[uint32_t(type)].get();
}

Object Device::GetNativeQueue(ObjectType objectType, CommandQueue queue)
{
    if (objectType != object_types::D3D12_CommandQueue)
    {
        return nullptr;
    }

    if (queue >= CommandQueue::Count)
    {
        return nullptr;
    }

    Queue* pQueue = GetQueue(queue);

    if (!pQueue)
    {
        return nullptr;
    }

    return Object(pQueue->queue.get());
}

bool Device::QueryFeatureSupport(Feature feature, void* pInfo, size_t infoSize)
{
    switch (feature)
    {
    case Feature::DeferredCommandLists:
        return true;
    case Feature::SinglePassStereo:
        return m_singlePassStereoSupported;
    case Feature::RayTracingAccelStruct:
        return m_rayTracingSupported;
    case Feature::RayTracingPipeline:
        return m_rayTracingSupported;
    case Feature::RayTracingOpacityMicromap:
        return m_opacityMicromapSupported;
    case Feature::RayQuery:
        return m_traceRayInlineSupported;
    case Feature::FastGeometryShader:
        return m_fastGeometryShaderSupported;
    case Feature::ShaderExecutionReordering:
        return m_shaderExecutionReorderingSupported;
    case Feature::Meshlets:
        return m_meshletsSupported;
    case Feature::VariableRateShading:
        if (pInfo)
        {
            if (infoSize == sizeof(VariableRateShadingFeatureInfo))
            {
                auto* pVrsInfo = reinterpret_cast<VariableRateShadingFeatureInfo*>(pInfo);
                pVrsInfo->shadingRateImageTileSize = m_options6.ShadingRateImageTileSize;
            }
            else
            {
                SLUG_LOG(RHILogCategory, core::LogType::Alert, "NotSuported");
            }
        }
        return m_variableRateShadingSupported;
    case Feature::VirtualResources:
        return true;
    case Feature::ComputeQueue:
        return (GetQueue(CommandQueue::Compute) != nullptr);
    case Feature::CopyQueue:
        return (GetQueue(CommandQueue::Copy) != nullptr);
    case Feature::ConservativeRasterization:
        return true;
    case Feature::ConstantBufferRanges:
        return true;
    default:
        return false;
    }
}

FormatSupportBitset Device::QueryFormatSupport(FormatType format)
{
    const DxgiFormatMapping& formatMapping = D3D12Utility::GetDxgiFormatMapping(format);

    FormatSupportBitset result = FormatSupport::None;

    D3D12_FEATURE_DATA_FORMAT_SUPPORT featureData = {};
    featureData.Format = formatMapping.rtvFormat;

    m_context.device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &featureData, sizeof(featureData));

    if (featureData.Support1 & D3D12_FORMAT_SUPPORT1_BUFFER)
    {
        result.Set(FormatSupport::Buffer);
    }

    if (featureData.Support1 & (D3D12_FORMAT_SUPPORT1_TEXTURE1D | D3D12_FORMAT_SUPPORT1_TEXTURE2D | D3D12_FORMAT_SUPPORT1_TEXTURE3D | D3D12_FORMAT_SUPPORT1_TEXTURECUBE))
    {
        result.Set(FormatSupport::Texture);
    }

    if (featureData.Support1 & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL)
    {
        result.Set(FormatSupport::DepthStencil);
    }

    if (featureData.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET)
    {
        result.Set(FormatSupport::RenderTarget);
    }

    if (featureData.Support1 & D3D12_FORMAT_SUPPORT1_BLENDABLE)
    {
        result.Set(FormatSupport::Blendable);
    }

    if (formatMapping.srvFormat != featureData.Format)
    {
        featureData.Format = formatMapping.srvFormat;
        featureData.Support1 = (D3D12_FORMAT_SUPPORT1)0;
        featureData.Support2 = (D3D12_FORMAT_SUPPORT2)0;
        m_context.device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &featureData, sizeof(featureData));
    }

    if (featureData.Support1 & D3D12_FORMAT_SUPPORT1_IA_INDEX_BUFFER)
    {
        result.Set(FormatSupport::IndexBuffer);
    }

    if (featureData.Support1 & D3D12_FORMAT_SUPPORT1_IA_VERTEX_BUFFER)
    {
        result.Set(FormatSupport::VertexBuffer);
    }

    if (featureData.Support1 & D3D12_FORMAT_SUPPORT1_SHADER_LOAD)
    {
        result.Set(FormatSupport::ShaderLoad);
    }

    if (featureData.Support1 & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE)
    {
        result.Set(FormatSupport::ShaderSample);
    }

    if (featureData.Support2 & D3D12_FORMAT_SUPPORT2_UAV_ATOMIC_ADD)
    {
        result.Set(FormatSupport::ShaderAtomic);
    }

    if (featureData.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD)
    {
        result.Set(FormatSupport::ShaderUavLoad);
    }

    if (featureData.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE)
    {
        result.Set(FormatSupport::ShaderUavStore);
    }

    return result;
}

HeapPtr Device::CreateHeap(const HeapDesc& d)
{
    D3D12_HEAP_DESC heapDesc;
    heapDesc.SizeInBytes = d.capacity;
    heapDesc.Alignment = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
    heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapDesc.Properties.CreationNodeMask = 1;
    heapDesc.Properties.VisibleNodeMask = 1;

    if (m_options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1)
    {
        heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
    }
    else
    {
        heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
    }

    switch (d.type)
    {
    case HeapType::DeviceLocal:
        heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
        break;
    case HeapType::Upload:
        heapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
        break;
    case HeapType::ReadBack:
        heapDesc.Properties.Type = D3D12_HEAP_TYPE_READBACK;
        break;
    default:
        core::String str = core::StringUtility::Sprintfs("InvalidEnum HeapType %d", d.type);
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        return nullptr;
    }

    core::TReferencePtr<ID3D12Heap> d3dHeap;
    const HRESULT res = m_context.device->CreateHeap(&heapDesc, IID_PPV_ARGS(&d3dHeap));

    if (FAILED(res))
    {
        core::String str = core::StringUtility::Sprintfs("CreateHeap call failed for heap %s, HRESULT = %x", d.debugName.c_str(), res);
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        return nullptr;
    }

    if (!d.debugName.empty())
    {
        core::WString wname = core::StringUtility::GetWstring(d.debugName.c_str());
        d3dHeap->SetName(wname.c_str());
    }

    Heap* heap = new Heap();
    heap->heap = d3dHeap;
    heap->desc = d;
    return HeapPtr::create(heap);
}

TexturePtr Device::CreateTexture(const TextureDesc& d)
{
    D3D12_RESOURCE_DESC rd = D3D12Utility::ConvertTextureDesc(d);
    D3D12_HEAP_PROPERTIES heapProps = {};
    D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

    bool isShared = false;
    if (d.sharedResourceFlags.Test(SharedResourceFlags::Shared))
    {
        heapFlags |= D3D12_HEAP_FLAG_SHARED;
        isShared = true;
    }
    if (d.sharedResourceFlags.Test(SharedResourceFlags::Shared_CrossAdapter))
    {
        rd.Flags |= D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
        heapFlags |= D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER;
        isShared = true;
    }

    Texture* texture = new Texture(m_context, m_resources, d, rd);

    if (d.virtualTexture)
    {
        return TexturePtr::create(texture);
    }

    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_CLEAR_VALUE clearValue = D3D12Utility::ConvertTextureClearValue(d);

    HRESULT hr = m_context.device->CreateCommittedResource(
        &heapProps,
        heapFlags,
        &texture->resourceDesc,
        D3D12Utility::ConvertResourceStates(d.initialState),
        d.useClearValue ? &clearValue : nullptr,
        IID_PPV_ARGS(&texture->resource));

    if (FAILED(hr))
    {
        core::String str = core::StringUtility::Sprintfs("Failed to create texture %s, error code = %x", d.debugName.c_str(), hr);
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        SLUG_SAFE_DELETE(texture);
        return nullptr;
    }

    if (isShared)
    {
        hr = m_context.device->CreateSharedHandle(texture->resource, nullptr, GENERIC_ALL, nullptr, &texture->sharedHandle);
        if (FAILED(hr))
        {
            core::String str = core::StringUtility::Sprintfs("Failed to create shared handle %s, error code = %x", d.debugName.c_str(), hr);
            SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
            SLUG_SAFE_DELETE(texture);
            return nullptr;
        }
    }

    texture->PostCreate();

    return TexturePtr::create(texture);
}

MemoryRequirements Device::GetTextureMemoryRequirements(ITexture* texture)
{
    Texture* tmpTexture = core::MemoryUtilities::CheckedCast<Texture*>(texture);

    D3D12_RESOURCE_ALLOCATION_INFO allocInfo = m_context.device->GetResourceAllocationInfo(1, 1, &tmpTexture->resourceDesc);

    MemoryRequirements memReq;
    memReq.alignment = allocInfo.Alignment;
    memReq.size = allocInfo.SizeInBytes;
    return memReq;
}

bool Device::BindTextureMemory(ITexture* itexture, IHeap* iheap, uint64_t offset)
{
    Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(itexture);
    Heap* heap = core::MemoryUtilities::CheckedCast<Heap*>(iheap);

    if (texture && texture->resource)
    {
        return false;
    }

    if (!itexture->GetDesc().virtualTexture)
    {
        return false;
    }


    D3D12_CLEAR_VALUE clearValue = D3D12Utility::ConvertTextureClearValue(texture->desc);

    HRESULT hr = m_context.device->CreatePlacedResource(
        heap->heap, offset,
        &texture->resourceDesc,
        D3D12Utility::ConvertResourceStates(texture->desc.initialState),
        texture->desc.useClearValue ? &clearValue : nullptr,
        IID_PPV_ARGS(&texture->resource));

    if (FAILED(hr))
    {
        core::String str = core::StringUtility::Sprintfs("Failed to create placed texture %s, errod code = %x", texture->desc.debugName.c_str(), hr);
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        return false;
    }

    texture->heap = heap;
    texture->PostCreate();

    return true;
}

TexturePtr Device::CreateHandleForNativeTexture(ObjectType objectType, Object itexture, const TextureDesc& desc)
{
    if (itexture.pointer == nullptr)
    {
        return nullptr;
    }

    if (objectType != object_types::D3D12_Resource)
    {
        return nullptr;
    }

    ID3D12Resource* pResource = static_cast<ID3D12Resource*>(itexture.pointer);

    Texture* texture = new Texture(m_context, m_resources, desc, pResource->GetDesc());
    texture->resource = pResource;
    texture->PostCreate();

    return TexturePtr::create(texture);
}

StagingTexturePtr Device::CreateStagingTexture(const TextureDesc& d, CpuAccessMode cpuAccess)
{
    SLUG_ASSERT(cpuAccess != CpuAccessMode::None);

    StagingTexture* ret = new StagingTexture();
    ret->desc = d;
    ret->resourceDesc = D3D12Utility::ConvertTextureDesc(d);
    ret->ComputeSubresourceOffsets(m_context.device);

    BufferDesc bufferDesc;
    bufferDesc.byteSize = ret->GetSizeInBytes(m_context.device);
    bufferDesc.structStride = 0;
    bufferDesc.debugName = d.debugName;
    bufferDesc.cpuAccess = cpuAccess;

    BufferPtr buffer = CreateBuffer(bufferDesc);
    ret->buffer = core::MemoryUtilities::CheckedCast<Buffer*>(buffer.get());
    if (!ret->buffer)
    {
        delete ret;
        return nullptr;
    }

    ret->cpuAccess = cpuAccess;
    return StagingTexturePtr::create(ret);
}

void* Device::MapStagingTexture(IStagingTexture* itex, const TextureSlice& slice, CpuAccessMode cpuAccess, size_t* outRowPitch)
{
    StagingTexture* tex = core::MemoryUtilities::CheckedCast<StagingTexture*>(itex);

    SLUG_ASSERT(slice.x == 0);
    SLUG_ASSERT(slice.y == 0);
    SLUG_ASSERT(cpuAccess != CpuAccessMode::None);
    SLUG_ASSERT(tex->mappedRegion.size == 0);
    SLUG_ASSERT(tex->mappedAccess == CpuAccessMode::None);

    auto resolvedSlice = slice.Resolve(tex->desc);
    auto region = tex->GetSliceRegion(m_context.device, resolvedSlice);

    if (tex->lastUseFence)
    {
        D3D12Utility::WaitForFence(tex->lastUseFence, tex->lastUseFenceValue, m_fenceEvent);
        tex->lastUseFence = nullptr;
    }

    D3D12_RANGE range = {};

    if (cpuAccess == CpuAccessMode::Read)
    {
        range = { SIZE_T(region.offset), region.offset + region.size };
    } else
    {
        range = { 0, 0 };
    }

    uint8_t* ret;
    const HRESULT hr = tex->buffer->resource->Map(0, &range, (void**)&ret);

    if (FAILED(hr))
    {
        core::String str = core::StringUtility::Sprintfs("Map call failed for textre %s, error code %x", tex->desc.debugName.c_str(), hr);
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        return nullptr;
    }

    tex->mappedRegion = region;
    tex->mappedAccess = cpuAccess;

    *outRowPitch = region.footprint.Footprint.RowPitch;
    return ret + tex->mappedRegion.offset;
}

void Device::UnmapStagingTexture(IStagingTexture* itex)
{
    StagingTexture* tex = core::MemoryUtilities::CheckedCast<StagingTexture*>(itex);

    SLUG_ASSERT(tex->mappedRegion.size != 0);
    SLUG_ASSERT(tex->mappedAccess != CpuAccessMode::None);

    D3D12_RANGE range = {};

    if (tex->mappedAccess == CpuAccessMode::Write)
    {
        range = { SIZE_T(tex->mappedRegion.offset), tex->mappedRegion.offset + tex->mappedRegion.size };
    } else
    {
        range = { 0, 0 };
    }

    tex->buffer->resource->Unmap(0, &range);

    tex->mappedRegion.size = 0;
    tex->mappedAccess = CpuAccessMode::None;
}

SamplerPtr Device::CreateSampler(const SamplerDesc& d)
{
    Sampler* sampler = new Sampler(m_context, d);
    return SamplerPtr::create(sampler);
}

BufferPtr Device::CreateBuffer(const BufferDesc& d)
{
    BufferDesc desc = d;
    if (desc.useConstantBuffer)
    {
        desc.byteSize = TAlign(d.byteSize, 256ull);
    }

    Buffer* buffer = new Buffer(m_context, m_resources, desc);

    if (d.volatileBuffer)
    {
        return BufferPtr::create(buffer);
    }

    D3D12_RESOURCE_DESC& resourceDesc = buffer->resourceDesc;
    resourceDesc.Width = buffer->desc.byteSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (buffer->desc.useUavs)
    {
        resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    if (d.virtualBuffer)
    {
        return BufferPtr::create(buffer);
    }

    D3D12_HEAP_PROPERTIES heapProps = {};
    D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;
    D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

    bool isShared = false;
    if (d.sharedResourceFlags.Test(SharedResourceFlags::Shared))
    {
        heapFlags |= D3D12_HEAP_FLAG_SHARED;
        isShared = true;
    }

    if (d.sharedResourceFlags.Test(SharedResourceFlags::Shared_CrossAdapter))
    {
        resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
        heapFlags |= D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER;
        isShared = true;
    }

    switch (buffer->desc.cpuAccess)
    {
    case CpuAccessMode::None:
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        initialState = D3D12Utility::ConvertResourceStates(d.initialState);
        break;

    case CpuAccessMode::Read:
        heapProps.Type = D3D12_HEAP_TYPE_READBACK;
        initialState = D3D12_RESOURCE_STATE_COPY_DEST;
        break;

    case CpuAccessMode::Write:
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
        break;
    }

    HRESULT hr = m_context.device->CreateCommittedResource(&heapProps, heapFlags, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&buffer->resource));

    if (FAILED(hr))
    {
        core::String str = core::StringUtility::Sprintfs("CreateCommittedResource call failed for buffer %s, error code %x", d.debugName.c_str(), hr);
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        SLUG_SAFE_DELETE(buffer);
        return nullptr;
    }

    if (isShared)
    {
        hr = m_context.device->CreateSharedHandle(buffer->resource, nullptr, GENERIC_ALL, nullptr, &buffer->sharedHandle);

        if (FAILED(hr))
        {
            core::String str = core::StringUtility::Sprintfs("Failed to create shared handle %s, error code = %x", d.debugName.c_str(), hr);
            SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
            SLUG_SAFE_DELETE(buffer);
            return nullptr;
        }
    }

    buffer->PostCreate();

    return BufferPtr::create(buffer);
}

void* Device::MapBuffer(IBuffer* ib, CpuAccessMode mapFlags)
{
    Buffer* b = core::MemoryUtilities::CheckedCast<Buffer*>(ib);

    if (b->lastUseFence)
    {
        D3D12Utility::WaitForFence(b->lastUseFence, b->lastUseFenceValue, m_fenceEvent);
        b->lastUseFence = nullptr;
    }

    D3D12_RANGE range;

    if (mapFlags == CpuAccessMode::Read)
    {
        range = { 0, b->desc.byteSize };
    }
    else
    {
        range = { 0, 0 };
    }

    void* mappedBuffer;
    const HRESULT hr = b->resource->Map(0, &range, &mappedBuffer);

    if (FAILED(hr))
    {
        core::String str = core::StringUtility::Sprintfs("Map call failed for buffer %s, error code %x", b->desc.debugName.c_str(), hr);
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        return nullptr;
    }

    return mappedBuffer;
}

void Device::UnmapBuffer(IBuffer* ib)
{
    Buffer* b = core::MemoryUtilities::CheckedCast<Buffer*>(ib);
    b->resource->Unmap(0, nullptr);
}

MemoryRequirements Device::GetBufferMemoryRequirements(IBuffer* ibuffer)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);

    D3D12_RESOURCE_ALLOCATION_INFO allocInfo = m_context.device->GetResourceAllocationInfo(1, 1, &buffer->resourceDesc);

    MemoryRequirements memReq;
    memReq.alignment = allocInfo.Alignment;
    memReq.size = allocInfo.SizeInBytes;
    return memReq;
}

bool Device::BindBufferMemory(IBuffer* ibuffer, IHeap* iheap, uint64_t offset)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);
    Heap* heap = core::MemoryUtilities::CheckedCast<Heap*>(iheap);

    if (buffer->resource)
    {
        return false;
    }

    if (!buffer->desc.virtualBuffer)
    {
        return false;
    }

    HRESULT hr = m_context.device->CreatePlacedResource(heap->heap, offset, &buffer->resourceDesc, D3D12Utility::ConvertResourceStates(buffer->desc.initialState), nullptr, IID_PPV_ARGS(&buffer->resource));
    if (FAILED(hr))
    {
        core::String str = core::StringUtility::Sprintfs("Failed to create placed buffer %s, error code %x", buffer->desc.debugName.c_str(), hr);
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        return false;
    }

    buffer->heap = heap;
    buffer->PostCreate();

    return true;
}

BufferPtr Device::CreateHandleForNativeBuffer(ObjectType objectType, Object ibuffer, const BufferDesc& desc)
{
    if (ibuffer.pointer == nullptr)
    {
        return nullptr;
    }

    if (objectType != object_types::D3D12_Resource)
    {
        return nullptr;
    }

    ID3D12Resource* pResource = static_cast<ID3D12Resource*>(ibuffer.pointer);

    Buffer* buffer = new Buffer(m_context, m_resources, desc);
    buffer->resource = pResource;

    buffer->PostCreate();

    return BufferPtr::create(buffer);
}

ShaderPtr Device::CreateShader(const ShaderDesc& desc, const void* binary, size_t binarySize)
{
    // バイナリサイズ確認
    if (binarySize == 0)
    {
        return nullptr;
    }

    // シェーダのバイナリデータをコピー
    Shader* shader = new Shader();
    shader->bytecode.resize(binarySize);
    shader->desc = desc;
    memcpy(&shader->bytecode[0], binary, binarySize);

    if (desc.numCustomSemantics || desc.coordinateSwizzling || (desc.fastGeometryFlags != FastGeometryShaderFlags::ForceFastGS) || desc.hlslExtensionsUav >= 0)
    {
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "NotSuported");
        SLUG_SAFE_DELETE(shader);
        return nullptr;
    }

    // シェーダを生成
    return ShaderPtr::create(shader);
}

ShaderPtr Device::CreateShaderSpecialization(IShader* baseShader, const ShaderSpecialization* constants, uint32_t numConstants)
{
    SLUG_LOG(RHILogCategory, core::LogType::Alert, "NotSuported");
    return nullptr;
}

ShaderLibraryPtr Device::CreateShaderLibrary(const void* binary, size_t binarySize)
{
    ShaderLibrary* shaderLibrary = new ShaderLibrary();

    shaderLibrary->bytecode.resize(binarySize);
    memcpy(&shaderLibrary->bytecode[0], binary, binarySize);

    return ShaderLibraryPtr::create(shaderLibrary);
}

bool Device::SetHlslExtensionsUAV(uint32_t slot)
{
    (void)slot;
    SLUG_LOG(RHILogCategory, core::LogType::Alert, "This version of NVRHI does not support NVIDIA HLSL extensions, please rebuild with NVAPI.");
    return false;
}

InputLayoutPtr Device::CreateInputLayout(const VertexAttributeDesc* desc, uint32_t attributeCount, IShader* vertexShader)
{
    (void)vertexShader;

    InputLayout* layout = new InputLayout();
    layout->attributes.resize(attributeCount);

    for (uint32_t index = 0; index < attributeCount; index++)
    {
        VertexAttributeDesc& attr = layout->attributes[index];

        attr = desc[index];

        SLUG_ASSERT(attr.arraySize > 0);

        const DxgiFormatMapping& formatMapping = D3D12Utility::GetDxgiFormatMapping(attr.format);
        const FormatInfo& formatInfo = GetFormatInfo(attr.format);

        for (uint32_t semanticIndex = 0; semanticIndex < attr.arraySize; semanticIndex++)
        {
            D3D12_INPUT_ELEMENT_DESC input_desc;

            input_desc.SemanticName = attr.name.c_str();
            input_desc.AlignedByteOffset = static_cast<uint32_t>(attr.offset + semanticIndex * formatInfo.bytesPerBlock);
            input_desc.Format = formatMapping.srvFormat;
            input_desc.InputSlot = attr.bufferIndex;
            input_desc.SemanticIndex = semanticIndex;

            if (attr.instanced)
            {
                input_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
                input_desc.InstanceDataStepRate = 1;
            } else
            {
                input_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                input_desc.InstanceDataStepRate = 0;
            }

            layout->inputElements.push_back(input_desc);
        }

        if (layout->elementStrides.find(attr.bufferIndex) == layout->elementStrides.end())
        {
            layout->elementStrides[attr.bufferIndex] = attr.elementStride;
        }
        else
        {
            auto element_stride_itr = layout->elementStrides.find(attr.bufferIndex);
            if (element_stride_itr != layout->elementStrides.end())
            {
                SLUG_ASSERT(element_stride_itr->second == attr.elementStride);
            }
        }
    }

    return InputLayoutPtr::create(layout);
}

EventQueryPtr Device::CreateEventQuery()
{
    EventQuery* ret = new EventQuery();
    return EventQueryPtr::create(ret);
}

void Device::SetEventQuery(IEventQuery* iquery, CommandQueue queue)
{
    EventQuery* query = core::MemoryUtilities::CheckedCast<EventQuery*>(iquery);
    Queue* pQueue = GetQueue(queue);

    query->started = true;
    query->fence = pQueue->fence;
    query->fenceCounter = pQueue->lastSubmittedInstance;
    query->resolved = false;
}

bool Device::PollEventQuery(IEventQuery* iquery)
{
    EventQuery* query = core::MemoryUtilities::CheckedCast<EventQuery*>(iquery);

    if (!query->started)
    {
        return false;
    }

    if (query->resolved)
    {
        return true;
    }

    SLUG_ASSERT(query->fence);

    if (query->fence->GetCompletedValue() >= query->fenceCounter)
    {
        query->resolved = true;
        query->fence = nullptr;
    }

    return query->resolved;
}

void Device::WaitEventQuery(IEventQuery* iquery)
{
    EventQuery* query = core::MemoryUtilities::CheckedCast<EventQuery*>(iquery);

    if (!query->started || query->resolved)
    {
        return;
    }

    SLUG_ASSERT(query->fence);

    D3D12Utility::WaitForFence(query->fence, query->fenceCounter, m_fenceEvent);
}

void Device::ResetEventQuery(IEventQuery* iquery)
{
    EventQuery* query = core::MemoryUtilities::CheckedCast<EventQuery*>(iquery);

    query->started = false;
    query->resolved = false;
    query->fence = nullptr;
}

TimerQueryPtr Device::CreateTimerQuery()
{
    if (!m_context.timerQueryHeap)
    {
        core::LockGuard lockGuard(m_mutex);

        if (!m_context.timerQueryHeap)
        {
            D3D12_QUERY_HEAP_DESC queryHeapDesc = {};
            queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
            queryHeapDesc.Count = uint32_t(m_resources.timerQueries.GetCapacity()) * 2;
            m_context.device->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&m_context.timerQueryHeap));

            BufferDesc qbDesc;
            qbDesc.byteSize = queryHeapDesc.Count * 8;
            qbDesc.cpuAccess = CpuAccessMode::Read;

            BufferPtr timerQueryBuffer = CreateBuffer(qbDesc);
            m_context.timerQueryResolveBuffer = core::MemoryUtilities::CheckedCast<Buffer*>(timerQueryBuffer.get());
        }
    }

    int32_t queryIndex = m_resources.timerQueries.Allocate();

    if (queryIndex < 0)
    {
        return nullptr;
    }

    TimerQuery* query = new TimerQuery(m_resources);
    query->beginQueryIndex = uint32_t(queryIndex) * 2;
    query->endQueryIndex = query->beginQueryIndex + 1;
    query->resolved = false;
    query->time = 0.f;

    return TimerQueryPtr::create(query);
}

bool Device::PollTimerQuery(ITimerQuery* iquery)
{
    TimerQuery* query = core::MemoryUtilities::CheckedCast<TimerQuery*>(iquery);

    if (!query->started)
    {
        return false;
    }

    if (!query->fence)
    {
        return true;
    }

    if (query->fence->GetCompletedValue() >= query->fenceCounter)
    {
        query->fence = nullptr;
        return true;
    }

    return false;
}

float Device::GetTimerQueryTime(ITimerQuery* iquery)
{
    TimerQuery* query = core::MemoryUtilities::CheckedCast<TimerQuery*>(iquery);

    if (!query->resolved)
    {
        if (query->fence)
        {
            D3D12Utility::WaitForFence(query->fence, query->fenceCounter, m_fenceEvent);
            query->fence = nullptr;
        }

        uint64_t frequency;
        GetQueue(CommandQueue::Graphics)->queue->GetTimestampFrequency(&frequency);

        D3D12_RANGE bufferReadRange = { query->beginQueryIndex * sizeof(uint64_t), (query->beginQueryIndex + 2) * sizeof(uint64_t) };
        uint64_t* data;
        const HRESULT res = m_context.timerQueryResolveBuffer->resource->Map(0, &bufferReadRange, (void**)&data);

        if (FAILED(res))
        {
            SLUG_LOG(RHILogCategory, core::LogType::Alert, "getTimerQueryTime: Map() failed");
            return 0.f;
        }

        query->resolved = true;
        query->time = float(double(data[query->endQueryIndex] - data[query->beginQueryIndex]) / double(frequency));

        m_context.timerQueryResolveBuffer->resource->Unmap(0, nullptr);
    }

    return query->time;
}

void Device::ResetTimerQuery(ITimerQuery* iquery)
{
    TimerQuery* query = core::MemoryUtilities::CheckedCast<TimerQuery*>(iquery);

    query->started = false;
    query->resolved = false;
    query->time = 0.f;
    query->fence = nullptr;
}

FrameBufferPtr Device::CreateFrameBuffer(const FrameBufferDesc& desc)
{
    FrameBuffer* fb = new FrameBuffer(m_resources);
    fb->desc = desc;
    fb->frameBufferInfo = FrameBufferInfoEx(desc);

    if (!desc.colorAttachments.empty())
    {
        Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(desc.colorAttachments[0].texture);
        fb->rtWidth = texture->desc.width;
        fb->rtHeight = texture->desc.height;
    }
    else if (desc.depthAttachment.Valid())
    {
        Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(desc.depthAttachment.texture);
        fb->rtWidth = texture->desc.width;
        fb->rtHeight = texture->desc.height;
    }

    for (size_t rt = 0; rt < desc.colorAttachments.size(); rt++)
    {
        auto& attachment = desc.colorAttachments[rt];

        Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(attachment.texture);
        SLUG_ASSERT(texture->desc.width == fb->rtWidth);
        SLUG_ASSERT(texture->desc.height == fb->rtHeight);

        DescriptorIndex index = m_resources.renderTargetViewHeap.AllocateDescriptor();

        const D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_resources.renderTargetViewHeap.GetCpuHandle(index);
        texture->CreateRTV(descriptorHandle.ptr, attachment.format, attachment.subresources);

        fb->rtvs.push_back(index);
        fb->textures.push_back(texture);
    }

    if (desc.depthAttachment.Valid())
    {
        Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(desc.depthAttachment.texture);
        SLUG_ASSERT(texture->desc.width == fb->rtWidth);
        SLUG_ASSERT(texture->desc.height == fb->rtHeight);

        DescriptorIndex index = m_resources.depthStencilViewHeap.AllocateDescriptor();

        const D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_resources.depthStencilViewHeap.GetCpuHandle(index);
        texture->CreateDSV(descriptorHandle.ptr, desc.depthAttachment.subresources, desc.depthAttachment.readOnly);

        fb->dsv = index;
        fb->textures.push_back(texture);
    }

    return FrameBufferPtr::create(fb);
}

core::TReferencePtr<RootSignature> Device::GetRootSignature(const core::TStaticVector<BindingLayoutPtr, MAX_BINDING_LAYOUTS>& pipelineLayouts, bool allowInputLayout, core::TReferencePtr<RootSignature> pRS)
{
    size_t hash = 0;

    for (const BindingLayoutPtr& pipelineLayout : pipelineLayouts)
    {
        HashCombine(hash, pipelineLayout.get());
    }

    HashCombine(hash, allowInputLayout ? 1u : 0u);

    core::TReferencePtr<RootSignature> rootsig = nullptr;
    auto rootsig_itr = m_resources.rootsignatureCache.find(hash);
    if (rootsig_itr != m_resources.rootsignatureCache.end())
    {
        rootsig = rootsig_itr->second;
    }

    if (!rootsig && pRS)
    {
        rootsig = pRS;
        m_resources.rootsignatureCache[hash] = rootsig;
    }

    if (!rootsig)
    {
        rootsig = core::MemoryUtilities::CheckedCast<RootSignature*>(BuildRootSignature(pipelineLayouts, allowInputLayout, false).get());
        rootsig->hash = hash;

        m_resources.rootsignatureCache[hash] = rootsig;
    }

    return rootsig;
}

GraphicsPipelinePtr Device::CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, IFrameBuffer* fb)
{
    core::TReferencePtr<RootSignature> pRS = GetRootSignature(desc.bindingLayouts, desc.inputLayout != nullptr);
    core::TReferencePtr<ID3D12PipelineState> pPSO = CreatePipelineState(desc, pRS, fb->GetFrameBufferInfo());

    return CreateHandleForNativeGraphicsPipeline(pRS, pPSO, desc, fb->GetFrameBufferInfo());
}

GraphicsPipelinePtr Device::CreateGraphicsPipelineFromCache(const PipelineCache& pipelineCache, GraphicsPipelineDesc& outDesc)
{
    core::TReferencePtr<RootSignature> pRS;
    FrameBufferInfoEx frameBufferInfo {};
    core::TReferencePtr<ID3D12PipelineState> pPSO = CreatePipelineStateFromCache(pipelineCache, outDesc, pRS, frameBufferInfo);
    pRS = GetRootSignature(outDesc.bindingLayouts, outDesc.inputLayout != nullptr);
    return CreateHandleForNativeGraphicsPipeline(pRS, pPSO, outDesc, frameBufferInfo);
}

GraphicsPipelinePtr Device::CreateHandleForNativeGraphicsPipeline(IRootSignature* rootSignature, ID3D12PipelineState* pipelineState, const GraphicsPipelineDesc& desc, const FrameBufferInfo& framebufferInfo)
{
    if (rootSignature == nullptr)
    {
        return nullptr;
    }

    if (pipelineState == nullptr)
    {
        return nullptr;
    }

    GraphicsPipeline* pso = new GraphicsPipeline();
    pso->desc = desc;
    pso->framebufferInfo = framebufferInfo;
    pso->rootSignature = core::MemoryUtilities::CheckedCast<RootSignature*>(rootSignature);
    pso->pipelineState = pipelineState;
    pso->requiresBlendFactor = desc.renderState.blendState.GetUsesConstantColor(uint32_t(pso->framebufferInfo.colorFormats.size()));

    return GraphicsPipelinePtr::create(pso);
}

core::TReferencePtr<ID3D12PipelineState> Device::CreatePipelineState(const GraphicsPipelineDesc& desc, RootSignature* pRS, const FrameBufferInfo& fbinfo) const
{
    if (desc.renderState.singlePassStereo.enabled && !m_singlePassStereoSupported)
    {
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Single-pass stereo is not supported by this device");
        return nullptr;
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12Desc = {};
    d3d12Desc.pRootSignature = pRS->handle;

    Shader* shader;
    shader = core::MemoryUtilities::CheckedCast<Shader*>(desc.vs.get());
    if (shader)
    {
        d3d12Desc.VS = { &shader->bytecode[0], shader->bytecode.size() };
    }

    shader = core::MemoryUtilities::CheckedCast<Shader*>(desc.hs.get());
    if (shader)
    {
        d3d12Desc.HS = { &shader->bytecode[0], shader->bytecode.size() };
    }

    shader = core::MemoryUtilities::CheckedCast<Shader*>(desc.ds.get());
    if (shader)
    {
        d3d12Desc.DS = { &shader->bytecode[0], shader->bytecode.size() };
    }

    shader = core::MemoryUtilities::CheckedCast<Shader*>(desc.gs.get());
    if (shader)
    {
        d3d12Desc.GS = { &shader->bytecode[0], shader->bytecode.size() };
    }

    shader = core::MemoryUtilities::CheckedCast<Shader*>(desc.ps.get());
    if (shader)
    {
        d3d12Desc.PS = { &shader->bytecode[0], shader->bytecode.size() };
    }


    D3D12Utility::TranslateBlendState(desc.renderState.blendState, d3d12Desc.BlendState);


    const DepthStencilState& depthState = desc.renderState.depthStencilState;
    D3D12Utility::TranslateDepthStencilState(depthState, d3d12Desc.DepthStencilState);

    if ((depthState.depthTestEnable || depthState.stencilEnable) && fbinfo.depthFormat == FormatType::UNKNOWN)
    {
        d3d12Desc.DepthStencilState.DepthEnable = FALSE;
        d3d12Desc.DepthStencilState.StencilEnable = FALSE;
        SLUG_LOG(RHILogCategory, core::LogType::Warning, "depthEnable or stencilEnable is true, but no depth target is bound");
    }

    const RasterState& rasterState = desc.renderState.rasterState;
    D3D12Utility::TranslateRasterizerState(rasterState, d3d12Desc.RasterizerState);

    switch (desc.primitiveType)
    {
    case PrimitiveType::PointList:
        d3d12Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        break;
    case PrimitiveType::LineList:
        d3d12Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        break;
    case PrimitiveType::TriangleList:
    case PrimitiveType::TriangleStrip:
        d3d12Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        break;
    case PrimitiveType::PatchList:
        d3d12Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
        break;
    }

    d3d12Desc.DSVFormat = D3D12Utility::GetDxgiFormatMapping(fbinfo.depthFormat).rtvFormat;

    d3d12Desc.SampleDesc.Count = fbinfo.sampleCount;
    d3d12Desc.SampleDesc.Quality = fbinfo.sampleQuality;

    for (uint32_t i = 0; i < uint32_t(fbinfo.colorFormats.size()); i++)
    {
        d3d12Desc.RTVFormats[i] = D3D12Utility::GetDxgiFormatMapping(fbinfo.colorFormats[i]).rtvFormat;
    }

    InputLayout* inputLayout = core::MemoryUtilities::CheckedCast<InputLayout*>(desc.inputLayout.get());
    if (inputLayout && !inputLayout->inputElements.empty())
    {
        d3d12Desc.InputLayout.NumElements = uint32_t(inputLayout->inputElements.size());
        d3d12Desc.InputLayout.pInputElementDescs = &(inputLayout->inputElements[0]);
    }

    d3d12Desc.NumRenderTargets = uint32_t(fbinfo.colorFormats.size());
    d3d12Desc.SampleMask = ~0u;

    core::TReferencePtr<ID3D12PipelineState> pipelineState;
    const HRESULT hr = m_context.device->CreateGraphicsPipelineState(&d3d12Desc, IID_PPV_ARGS(&pipelineState));

    if (FAILED(hr))
    {
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Failed to create a graphics pipeline state object");
        return nullptr;
    }

    return pipelineState;
}

void Device::RegisterShaderCode(ShaderPtr& srcShader, const void* dstShaderBytecodePointer, size_t dstBytecodeLength) const
{
    Shader* shader = new Shader();
    shader->bytecode.resize(dstBytecodeLength);
    core::MemoryUtilities::Memcpy(&shader->bytecode[0], shader->bytecode.size(), dstShaderBytecodePointer, dstBytecodeLength);
    srcShader = shader;
}

core::TReferencePtr<ID3D12PipelineState> Device::CreatePipelineStateFromCache(const PipelineCache& pipelineCache, GraphicsPipelineDesc& outDesc, core::TReferencePtr<RootSignature>& outRootSignature, FrameBufferInfoEx& outFrameBufferInfo)
{
    core::TReferencePtr<ID3D12PipelineState> pipelineState = nullptr;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12Desc = {};
    if (pipelineCache.cache != nullptr && pipelineCache.cacheSize > 0)
    {
        core::TReferencePtr<ID3D12PipelineLibrary> pipelineLibrary = nullptr;
        HRESULT hr = m_context.device2->CreatePipelineLibrary(pipelineCache.cache, pipelineCache.cacheSize, IID_PPV_ARGS(&pipelineLibrary));
        if (SUCCEEDED(hr))
        {
            auto wstr = core::StringUtility::GetWstring(pipelineCache.cacheName.c_str());
            hr = pipelineLibrary->LoadGraphicsPipeline(wstr.c_str(), &d3d12Desc, IID_PPV_ARGS(&pipelineState));
        }
    }

    if (outRootSignature == nullptr)
    {
        outRootSignature.attach(new RootSignature(m_resources));
    }

    outRootSignature->handle = d3d12Desc.pRootSignature;

    if (d3d12Desc.VS.BytecodeLength > 0 && d3d12Desc.VS.pShaderBytecode != nullptr)
    {
        RegisterShaderCode(outDesc.vs, d3d12Desc.VS.pShaderBytecode, d3d12Desc.VS.BytecodeLength);
    }

    if (d3d12Desc.HS.BytecodeLength > 0 && d3d12Desc.HS.pShaderBytecode != nullptr)
    {
        RegisterShaderCode(outDesc.hs, d3d12Desc.HS.pShaderBytecode, d3d12Desc.HS.BytecodeLength);
    }

    if (d3d12Desc.DS.BytecodeLength > 0 && d3d12Desc.DS.pShaderBytecode != nullptr)
    {
        RegisterShaderCode(outDesc.ds, d3d12Desc.DS.pShaderBytecode, d3d12Desc.DS.BytecodeLength);
    }

    if (d3d12Desc.GS.BytecodeLength > 0 && d3d12Desc.GS.pShaderBytecode != nullptr)
    {
        RegisterShaderCode(outDesc.gs, d3d12Desc.GS.pShaderBytecode, d3d12Desc.GS.BytecodeLength);
    }

    if (d3d12Desc.PS.BytecodeLength > 0 && d3d12Desc.PS.pShaderBytecode != nullptr)
    {
        RegisterShaderCode(outDesc.ps, d3d12Desc.PS.pShaderBytecode, d3d12Desc.PS.BytecodeLength);
    }
   
    D3D12Utility::TranslateBlendState(d3d12Desc.BlendState, outDesc.renderState.blendState);


    D3D12Utility::TranslateDepthStencilState(d3d12Desc.DepthStencilState, outDesc.renderState.depthStencilState);

    D3D12Utility::TranslateRasterizerState(d3d12Desc.RasterizerState, outDesc.renderState.rasterState);

    outDesc.primitiveType = D3D12Utility::ConvertD3D12PrimitiveType(d3d12Desc.PrimitiveTopologyType);

    outFrameBufferInfo.depthFormat = D3D12Utility::ConvertFormat(d3d12Desc.DSVFormat, DxgiFormatMapping::UseType::RTV);
    outFrameBufferInfo.sampleCount = d3d12Desc.SampleDesc.Count;
    outFrameBufferInfo.sampleQuality = d3d12Desc.SampleDesc.Quality;

    for (uint32_t i = 0; i < uint32_t(d3d12Desc.NumRenderTargets); i++)
    {
        outFrameBufferInfo.colorFormats[i] = D3D12Utility::ConvertFormat(d3d12Desc.RTVFormats[i], DxgiFormatMapping::UseType::RTV);
    }

    outDesc.inputLayout = new InputLayout();
    InputLayout* inputLayout = core::MemoryUtilities::CheckedCast<InputLayout*>(outDesc.inputLayout.get());
    for (uint32_t i = 0; i < uint32_t(d3d12Desc.InputLayout.NumElements); i++)
    {
        inputLayout->inputElements.push_back(d3d12Desc.InputLayout.pInputElementDescs[i]);
    }

    return pipelineState;
}

ComputePipelinePtr Device::CreateComputePipeline(const ComputePipelineDesc& desc)
{
    core::TReferencePtr<RootSignature> pRS = GetRootSignature(desc.bindingLayouts, false);
    core::TReferencePtr<ID3D12PipelineState> pCSO = CreatePipelineState(desc, pRS);

    if (pCSO == nullptr)
    {
        return nullptr;
    }

    ComputePipeline* cso = new ComputePipeline();


    cso->desc = desc;

    cso->rootSignature = pRS;
    cso->pipelineState = pCSO;

    return ComputePipelinePtr::create(cso);
}

ComputePipelinePtr Device::CreateComputePipelineFromCache(const PipelineCache& pipelineCache, ComputePipelineDesc& outDesc)
{
    core::TReferencePtr<RootSignature> pRS;
    core::TReferencePtr<ID3D12PipelineState> pCSO = CreatePipelineStateFromCache(pipelineCache, outDesc, pRS);
    pRS = GetRootSignature(outDesc.bindingLayouts,false);

    if (pCSO == nullptr)
    {
        return nullptr;
    }

    ComputePipeline* cso = new ComputePipeline();

    cso->desc = outDesc;

    cso->rootSignature = pRS;
    cso->pipelineState = pCSO;

    return ComputePipelinePtr::create(cso);

}

core::TReferencePtr<ID3D12PipelineState> Device::CreatePipelineState(const ComputePipelineDesc& desc, RootSignature* pRS) const
{
    core::TReferencePtr<ID3D12PipelineState> pipelineState;

    D3D12_COMPUTE_PIPELINE_STATE_DESC d3d12Desc = {};

    d3d12Desc.pRootSignature = pRS->handle;
    Shader* shader = core::MemoryUtilities::CheckedCast<Shader*>(desc.cs.get());
    d3d12Desc.CS = { &shader->bytecode[0], shader->bytecode.size() };

    const HRESULT hr = m_context.device->CreateComputePipelineState(&d3d12Desc, IID_PPV_ARGS(&pipelineState));

    if (FAILED(hr))
    {
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Failed to create a compute pipeline state object");
        return nullptr;
    }

    return pipelineState;
}

core::TReferencePtr<ID3D12PipelineState> Device::CreatePipelineStateFromCache(const PipelineCache& pipelineCache, ComputePipelineDesc& outDesc, core::TReferencePtr<RootSignature>& outRootSignature)
{
    core::TReferencePtr<ID3D12PipelineState> pipelineState = nullptr;
    D3D12_COMPUTE_PIPELINE_STATE_DESC d3d12Desc = {};
    if (pipelineCache.cache != nullptr && pipelineCache.cacheSize > 0)
    {
        core::TReferencePtr<ID3D12PipelineLibrary> pipelineLibrary = nullptr;
        HRESULT hr = m_context.device2->CreatePipelineLibrary(pipelineCache.cache, pipelineCache.cacheSize, IID_PPV_ARGS(&pipelineLibrary));
        if (SUCCEEDED(hr))
        {
            auto wstr = core::StringUtility::GetWstring(pipelineCache.cacheName.c_str());
            hr = pipelineLibrary->LoadComputePipeline(wstr.c_str(), &d3d12Desc, IID_PPV_ARGS(&pipelineState));
        }
    }

    if (outRootSignature == nullptr)
    {
        outRootSignature.attach(new RootSignature(m_resources));
    }

    outRootSignature->handle = d3d12Desc.pRootSignature;

    if (d3d12Desc.CS.BytecodeLength > 0 && d3d12Desc.CS.pShaderBytecode != nullptr)
    {
        RegisterShaderCode(outDesc.cs, d3d12Desc.CS.pShaderBytecode, d3d12Desc.CS.BytecodeLength);
    }
    return pipelineState;
}

MeshletPipelinePtr Device::CreateMeshletPipeline(const MeshletPipelineDesc& desc, IFrameBuffer* fb)
{
    core::TReferencePtr<RootSignature> pRS = GetRootSignature(desc.bindingLayouts, false);

    core::TReferencePtr<ID3D12PipelineState> pPSO = CreatePipelineState(desc, pRS, fb->GetFrameBufferInfo());

    return CreateHandleForNativeMeshletPipeline(pRS, pPSO, desc, fb->GetFrameBufferInfo());

}

MeshletPipelinePtr Device::CreateHandleForNativeMeshletPipeline(IRootSignature* rootSignature, ID3D12PipelineState* pipelineState, const MeshletPipelineDesc& desc, const FrameBufferInfo& framebufferInfo)
{
    if (rootSignature == nullptr)
    {
        return nullptr;
    }

    if (pipelineState == nullptr)
    {
        return nullptr;
    }

    MeshletPipeline* pso = new MeshletPipeline();
    pso->desc = desc;
    pso->framebufferInfo = framebufferInfo;
    pso->rootSignature = core::MemoryUtilities::CheckedCast<RootSignature*>(rootSignature);
    pso->pipelineState = pipelineState;
    pso->requiresBlendFactor = desc.renderState.blendState.GetUsesConstantColor(uint32_t(pso->framebufferInfo.colorFormats.size()));

    return MeshletPipelinePtr::create(pso);
}

core::TReferencePtr<ID3D12PipelineState> Device::CreatePipelineState(const MeshletPipelineDesc& desc, RootSignature* pRS, const FrameBufferInfo& fbinfo) const
{
    core::TReferencePtr<ID3D12PipelineState> pipelineState;
#pragma warning(push)
#pragma warning(disable: 4324)
    struct PSO_STREAM
    {
        typedef __declspec(align(sizeof(void*))) D3D12_PIPELINE_STATE_SUBOBJECT_TYPE ALIGNED_TYPE;

        ALIGNED_TYPE RootSignature_Type;        ID3D12RootSignature* RootSignature;
        ALIGNED_TYPE PrimitiveTopology_Type;    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
        ALIGNED_TYPE AmplificationShader_Type;  D3D12_SHADER_BYTECODE AmplificationShader;
        ALIGNED_TYPE MeshShader_Type;           D3D12_SHADER_BYTECODE MeshShader;
        ALIGNED_TYPE PixelShader_Type;          D3D12_SHADER_BYTECODE PixelShader;
        ALIGNED_TYPE RasterizerState_Type;      D3D12_RASTERIZER_DESC RasterizerState;
        ALIGNED_TYPE DepthStencilState_Type;    D3D12_DEPTH_STENCIL_DESC DepthStencilState;
        ALIGNED_TYPE BlendState_Type;           D3D12_BLEND_DESC BlendState;
        ALIGNED_TYPE SampleDesc_Type;           DXGI_SAMPLE_DESC SampleDesc;
        ALIGNED_TYPE SampleMask_Type;           UINT SampleMask;
        ALIGNED_TYPE RenderTargets_Type;        D3D12_RT_FORMAT_ARRAY RenderTargets;
        ALIGNED_TYPE DSVFormat_Type;            DXGI_FORMAT DSVFormat;
    } psoDesc = { };
#pragma warning(pop)

    psoDesc.RootSignature_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
    psoDesc.PrimitiveTopology_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY;
    psoDesc.AmplificationShader_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS;
    psoDesc.MeshShader_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS;
    psoDesc.PixelShader_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS;
    psoDesc.RasterizerState_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER;
    psoDesc.DepthStencilState_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL;
    psoDesc.BlendState_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND;
    psoDesc.SampleDesc_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC;
    psoDesc.SampleMask_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK;
    psoDesc.RenderTargets_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS;
    psoDesc.DSVFormat_Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT;

    psoDesc.RootSignature = pRS->handle;

    D3D12Utility::TranslateBlendState(desc.renderState.blendState, psoDesc.BlendState);

    const DepthStencilState& depthState = desc.renderState.depthStencilState;
    D3D12Utility::TranslateDepthStencilState(depthState, psoDesc.DepthStencilState);

    if ((depthState.depthTestEnable || depthState.stencilEnable) && fbinfo.depthFormat == FormatType::UNKNOWN)
    {
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
    }

    const RasterState& rasterState = desc.renderState.rasterState;
    D3D12Utility::TranslateRasterizerState(rasterState, psoDesc.RasterizerState);

    switch (desc.primitiveType)
    {
    case PrimitiveType::PointList:
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        break;
    case PrimitiveType::LineList:
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        break;
    case PrimitiveType::TriangleList:
    case PrimitiveType::TriangleStrip:
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        break;
    case PrimitiveType::PatchList:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Unsupported primitive topology for meshlets");
        return nullptr;
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Invalid Enum PrimitiveType");
        return nullptr;
    }

    psoDesc.SampleDesc.Count = fbinfo.sampleCount;
    psoDesc.SampleDesc.Quality = fbinfo.sampleQuality;
    psoDesc.SampleMask = ~0u;

    for (uint32_t i = 0; i < uint32_t(fbinfo.colorFormats.size()); i++)
    {
        psoDesc.RenderTargets.RTFormats[i] = D3D12Utility::GetDxgiFormatMapping(fbinfo.colorFormats[i]).rtvFormat;
    }
    psoDesc.RenderTargets.NumRenderTargets = uint32_t(fbinfo.colorFormats.size());

    psoDesc.DSVFormat = D3D12Utility::GetDxgiFormatMapping(fbinfo.depthFormat).rtvFormat;

    if (desc.as)
    {
        desc.as->GetByteCode(&psoDesc.AmplificationShader.pShaderBytecode, &psoDesc.AmplificationShader.BytecodeLength);
    }

    if (desc.ms)
    {
        desc.ms->GetByteCode(&psoDesc.MeshShader.pShaderBytecode, &psoDesc.MeshShader.BytecodeLength);
    }

    if (desc.ps)
    {
        desc.ps->GetByteCode(&psoDesc.PixelShader.pShaderBytecode, &psoDesc.PixelShader.BytecodeLength);
    }

    D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
    streamDesc.pPipelineStateSubobjectStream = &psoDesc;
    streamDesc.SizeInBytes = sizeof(psoDesc);

    HRESULT hr = m_context.device2->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&pipelineState));
    if (FAILED(hr))
    {
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Failed to create a meshlet pipeline state object");
        return nullptr;
    }

    return pipelineState;
}

rt::PipelinePtr Device::CreateRayTracingPipeline(const rt::PipelineDesc& desc)
{
    RayTracingPipeline* pso = new RayTracingPipeline(m_context);
    {
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Not Implement");
    }
    return rt::PipelinePtr::create(pso);
}

BindingLayoutPtr Device::CreateBindingLayout(const BindingLayoutDesc& desc)
{
    BindingLayout* ret = new BindingLayout(desc);
    return BindingLayoutPtr::create(ret);
}

BindingLayoutPtr Device::CreateBindlessLayout(const BindlessLayoutDesc& desc)
{
    BindlessLayout* ret = new BindlessLayout(desc);
    return BindingLayoutPtr::create(ret);
}

BindingSetPtr Device::CreateBindingSet(const BindingSetDesc& desc, IBindingLayout* layout)
{
    BindingSet* ret = new BindingSet(m_context, m_resources);
    ret->desc = desc;

    BindingLayout* pipelineLayout = core::MemoryUtilities::CheckedCast<BindingLayout*>(layout);
    ret->layout = pipelineLayout;

    ret->CreateDescriptors();

    return BindingSetPtr::create(ret);
}

DescriptorTablePtr Device::CreateDescriptorTable(IBindingLayout* layout)
{
    (void)layout;

    DescriptorTable* ret = new DescriptorTable(m_resources);
    ret->capacity = 0;
    ret->firstDescriptor = 0;

    return DescriptorTablePtr::create(ret);
}

void Device::ResizeDescriptorTable(IDescriptorTable* idescriptorTable, uint32_t capacity, bool keepContents)
{
    DescriptorTable* descriptorTable = core::MemoryUtilities::CheckedCast<DescriptorTable*>(idescriptorTable);

    if (capacity == descriptorTable->capacity)
    {
        return;
    }

    if (capacity < descriptorTable->capacity)
    {
        m_resources.shaderResourceViewHeap.ReleaseDescriptors(descriptorTable->firstDescriptor + capacity, descriptorTable->capacity - capacity);
        descriptorTable->capacity = capacity;
        return;
    }

    uint32_t originalFirst = descriptorTable->firstDescriptor;
    if (!keepContents && descriptorTable->capacity > 0)
    {
        m_resources.shaderResourceViewHeap.ReleaseDescriptors(descriptorTable->firstDescriptor, descriptorTable->capacity);
    }

    descriptorTable->firstDescriptor = m_resources.shaderResourceViewHeap.AllocateDescriptors(capacity);

    if (keepContents && descriptorTable->capacity > 0)
    {
        m_context.device->CopyDescriptorsSimple(descriptorTable->capacity,
            m_resources.shaderResourceViewHeap.GetCpuHandle(descriptorTable->firstDescriptor),
            m_resources.shaderResourceViewHeap.GetCpuHandle(originalFirst),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        m_context.device->CopyDescriptorsSimple(descriptorTable->capacity,
            m_resources.shaderResourceViewHeap.GetCpuHandleShaderVisible(descriptorTable->firstDescriptor),
            m_resources.shaderResourceViewHeap.GetCpuHandle(originalFirst),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        m_resources.shaderResourceViewHeap.ReleaseDescriptors(originalFirst, descriptorTable->capacity);
    }

    descriptorTable->capacity = capacity;
}

bool Device::WriteDescriptorTable(IDescriptorTable* idescriptorTable, const BindingSetItem& binding)
{
    DescriptorTable* descriptorTable = core::MemoryUtilities::CheckedCast<DescriptorTable*>(idescriptorTable);

    if (binding.slot >= descriptorTable->capacity)
    {
        return false;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_resources.shaderResourceViewHeap.GetCpuHandle(descriptorTable->firstDescriptor + binding.slot);

    switch (binding.type)
    {
    case ResourceType::None:
        Buffer::CreateNullSRV(descriptorHandle.ptr, FormatType::UNKNOWN, m_context);
        break;
    case ResourceType::Texture_SRV:
        {
            Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(binding.resourcePtr);
            texture->CreateSRV(descriptorHandle.ptr, binding.format, binding.dimension, binding.subresources);
            break;
        }
    case ResourceType::Texture_UAV:
        {
            Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(binding.resourcePtr);
            texture->CreateUAV(descriptorHandle.ptr, binding.format, binding.dimension, binding.subresources);
            break;
        }
    case ResourceType::TypedBuffer_SRV:
    case ResourceType::StructuredBuffer_SRV:
    case ResourceType::RawBuffer_SRV:
        {
            Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(binding.resourcePtr);
            buffer->CreateSRV(descriptorHandle.ptr, binding.format, binding.range, binding.type);
            break;
        }
    case ResourceType::TypedBuffer_UAV:
    case ResourceType::StructuredBuffer_UAV:
    case ResourceType::RawBuffer_UAV:
        {
            Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(binding.resourcePtr);
            buffer->CreateUAV(descriptorHandle.ptr, binding.format, binding.range, binding.type);
            break;
        }
    case ResourceType::ConstantBuffer:
        {
            Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(binding.resourcePtr);
            buffer->CreateCBV(descriptorHandle.ptr, binding.range);
            break;
        }
    case ResourceType::RayTracingAccelStruct:
        {
            AccelStruct* accelStruct = core::MemoryUtilities::CheckedCast<AccelStruct*>(binding.resourcePtr);
            accelStruct->CreateSRV(descriptorHandle.ptr);
            break;
        }

    case ResourceType::VolatileConstantBuffer:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Attempted to bind a volatile constant buffer to a bindless set.");
        return false;

    case ResourceType::Sampler:
    case ResourceType::PushConstants:
    case ResourceType::Count:
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Invalid Enum ResourceType");
        return false;
    }

    m_resources.shaderResourceViewHeap.CopyToShaderVisibleHeap(descriptorTable->firstDescriptor + binding.slot, 1);
    return true;
}

IDescriptorHeap* Device::GetDescriptorHeap(DescriptorHeapType heapType)
{
    switch (heapType)
    {
    case DescriptorHeapType::RenderTargetView:
        return &m_resources.renderTargetViewHeap;
    case DescriptorHeapType::DepthStencilView:
        return &m_resources.depthStencilViewHeap;
    case DescriptorHeapType::ShaderResrouceView:
        return &m_resources.shaderResourceViewHeap;
    case DescriptorHeapType::Sampler:
        return &m_resources.samplerHeap;
    }

    return nullptr;
}

RootSignaturePtr Device::BuildRootSignature(const core::TStaticVector<BindingLayoutPtr, MAX_BINDING_LAYOUTS>& pipelineLayouts, bool allowInputLayout, bool isLocal, const D3D12_ROOT_PARAMETER1* pCustomParameters, uint32_t numCustomParameters)
{
    HRESULT res;

    RootSignature* rootsig = new RootSignature(m_resources);

    core::TVector<D3D12_ROOT_PARAMETER1> rootParameters;

    for (uint32_t index = 0; index < numCustomParameters; index++)
    {
        rootParameters.push_back(pCustomParameters[index]);
    }

    for (uint32_t layoutIndex = 0; layoutIndex < uint32_t(pipelineLayouts.size()); layoutIndex++)
    {
        if (pipelineLayouts[layoutIndex]->GetDesc())
        {
            BindingLayout* layout = core::MemoryUtilities::CheckedCast<BindingLayout*>(pipelineLayouts[layoutIndex].get());
            RootParameterIndex rootParameterOffset = RootParameterIndex(rootParameters.size());

            rootsig->pipelineLayouts.push_back(core::MakePair(layout, rootParameterOffset));

            rootParameters.insert(rootParameters.end(), layout->rootParameters.begin(), layout->rootParameters.end());

            if (layout->pushConstantByteSize)
            {
                rootsig->pushConstantByteSize = layout->pushConstantByteSize;
                rootsig->rootParameterPushConstants = layout->rootParameterPushConstants + rootParameterOffset;
            }
        }
        else if (pipelineLayouts[layoutIndex]->GetBindlessDesc())
        {
            BindlessLayout* layout = core::MemoryUtilities::CheckedCast<BindlessLayout*>(pipelineLayouts[layoutIndex].get());
            RootParameterIndex rootParameterOffset = RootParameterIndex(rootParameters.size());

            rootsig->pipelineLayouts.push_back(core::MakePair(layout, rootParameterOffset));

            rootParameters.push_back(layout->rootParameter);
        }
    }

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
    rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (allowInputLayout)
    {
        rsDesc.Desc_1_1.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    }
    if (isLocal)
    {
        rsDesc.Desc_1_1.Flags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
    }

    if (!rootParameters.empty())
    {
        rsDesc.Desc_1_1.pParameters = rootParameters.data();
        rsDesc.Desc_1_1.NumParameters = UINT(rootParameters.size());
    }

    core::TReferencePtr<ID3DBlob> rsBlob;
    core::TReferencePtr<ID3DBlob> errorBlob;
    res = D3D12SerializeVersionedRootSignature(&rsDesc, &rsBlob, &errorBlob);

    if (FAILED(res))
    {
        core::String str = core::StringUtility::Sprintfs("D3D12SerializeVersionedRootSignature call failed, HRESULT %x, ", res);
        if (errorBlob)
        {
            str += (const char*)errorBlob->GetBufferPointer();
        }
        SLUG_LOG(RHILogCategory, core::LogType::Alert, str.c_str());
        return nullptr;
    }

    res = m_context.device->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rootsig->handle));

    if (FAILED(res))
    {
        core::String str = core::StringUtility::Sprintfs("CreateRootSignature call failed, HRESULT = %x", res);
        return nullptr;
    }

    return RootSignaturePtr::create(rootsig);
}

rt::OpacityMicromapPtr Device::CreateOpacityMicromap(const rt::OpacityMicromapDesc& desc)
{
    SLUG_LOG(RHILogCategory, core::LogType::Alert, "Not Support");
    return nullptr;
}

rt::AccelStructPtr Device::CreateAccelStruct(const rt::AccelStructDesc& desc)
{
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO ASPreBuildInfo = {};
    if (!GetAccelStructPreBuildInfo(ASPreBuildInfo, desc))
    {
        return nullptr;
    }

    AccelStruct* as = new AccelStruct(m_context);
    as->desc = desc;
    as->allowUpdate = (desc.buildFlags.Test(rt::AccelStructBuildFlags::AllowUpdate));

    SLUG_ASSERT(ASPreBuildInfo.ResultDataMaxSizeInBytes <= ~0u);

    bool need_buffer = true;

    if (need_buffer)
    {
        BufferDesc bufferDesc;
        bufferDesc.useUavs = true;
        bufferDesc.byteSize = ASPreBuildInfo.ResultDataMaxSizeInBytes;
        bufferDesc.initialState = desc.isTopLevel ? ResourceStates::AccelStructRead : ResourceStates::AccelStructBuildBlas;
        bufferDesc.keepInitialState = true;
        bufferDesc.accelStructStorage = true;
        bufferDesc.debugName = desc.debugName;
        bufferDesc.virtualBuffer = desc.isVirtual;
        BufferPtr buffer = CreateBuffer(bufferDesc);
        as->dataBuffer = core::MemoryUtilities::CheckedCast<Buffer*>(buffer.get());
    }

    for (auto& geometry : as->desc.bottomLevelGeometries)
    {
        SLUG_STATIC_ASSERT_MSG(offsetof(rt::GeometryTriangles, indexBuffer) == offsetof(rt::GeometryAABBs, buffer), "");
        SLUG_STATIC_ASSERT_MSG(offsetof(rt::GeometryTriangles, vertexBuffer) == offsetof(rt::GeometryAABBs, unused), "");

        geometry.geometryData.triangles.indexBuffer = nullptr;
        geometry.geometryData.triangles.vertexBuffer = nullptr;
    }

    return rt::AccelStructPtr::create(as);
}

MemoryRequirements Device::GetAccelStructMemoryRequirements(rt::IAccelStruct* ias)
{
    AccelStruct* as = core::MemoryUtilities::CheckedCast<AccelStruct*>(ias);

    if (as->dataBuffer)
    {
        return GetBufferMemoryRequirements(as->dataBuffer);
    }

    return MemoryRequirements();
}

bool Device::BindAccelStructMemory(rt::IAccelStruct* ias, IHeap* heap, uint64_t offset)
{
    AccelStruct* as = core::MemoryUtilities::CheckedCast<AccelStruct*>(ias);

    if (as->dataBuffer)
    {
        return BindBufferMemory(as->dataBuffer, heap, offset);
    }

    return false;
}

bool Device::GetAccelStructPreBuildInfo(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& outPreBuildInfo, const rt::AccelStructDesc& desc) const
{
    D3D12BuildRaytracingAccelerationStructureInputs ASInputs;
    D3D12Raytracing::FillAsInputDescForPreBuildInfo(ASInputs, desc);

    const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = ASInputs.GetAs();
    m_context.device5->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &outPreBuildInfo);
    return true;
}

rhi::CommandListPtr Device::CreateCommandList(const CommandListParameters& params)
{
    // コマンドキューが存在するか確認
    if (!GetQueue(params.queueType))
    {
        return nullptr;
    }

    return CommandListPtr::create(new CommandList(this, m_context, m_resources, params));
}

uint64_t Device::ExecuteCommandLists(rhi::ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue)
{
    m_commandListsToExecute.resize(numCommandLists);
    for (size_t i = 0; i < numCommandLists; i++)
    {
        m_commandListsToExecute[i] = core::MemoryUtilities::CheckedCast<CommandList*>(pCommandLists[i])->GetD3D12CommandList();
    }

    Queue* pQueue = GetQueue(executionQueue);

    pQueue->queue->ExecuteCommandLists(uint32_t(m_commandListsToExecute.size()), m_commandListsToExecute.data());
    pQueue->lastSubmittedInstance++;
    pQueue->queue->Signal(pQueue->fence, pQueue->lastSubmittedInstance);

    for (size_t i = 0; i < numCommandLists; i++)
    {
        auto instance = core::MemoryUtilities::CheckedCast<CommandList*>(pCommandLists[i])->Executed(pQueue);
        pQueue->commandListsInFlight.push_front(instance);
    }

    HRESULT hr = m_context.device->GetDeviceRemovedReason();
    if (FAILED(hr))
    {
        SLUG_LOG(RHILogCategory, core::LogType::Error, "Device Removed!");
    }

    return pQueue->lastSubmittedInstance;
}

void Device::QueueWaitForCommandList(CommandQueue waitQueue, CommandQueue executionQueue, uint64_t instance)
{
    Queue* pWaitQueue = GetQueue(waitQueue);
    Queue* pExecutionQueue = GetQueue(executionQueue);
    SLUG_ASSERT(instance <= pExecutionQueue->lastSubmittedInstance);

    pWaitQueue->queue->Wait(pExecutionQueue->fence, instance);
}

void Device::WaitForIdle()
{
    for (const auto& pQueue : m_queues)
    {
        if (!pQueue)
        {
            continue;
        }

        if (pQueue->UpdateLastCompletedInstance() < pQueue->lastSubmittedInstance)
        {
            D3D12Utility::WaitForFence(pQueue->fence, pQueue->lastSubmittedInstance, m_fenceEvent);
        }
    }
}
void Device::RunGarbageCollection()
{
    for (const auto& pQueue : m_queues)
    {
        if (!pQueue)
        {
            continue;
        }

        pQueue->UpdateLastCompletedInstance();

        while (!pQueue->commandListsInFlight.empty())
        {
            core::TReferencePtr<CommandListInstance> instance = pQueue->commandListsInFlight.back();

            if (pQueue->lastCompletedInstance >= instance->submittedInstance)
            {
                pQueue->commandListsInFlight.pop_back();
            }
            else
            {
                break;
            }
        }
    }
}

}
