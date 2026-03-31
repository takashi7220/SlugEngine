
#include "core/memory/MemoryUtilities.hpp"
#include "rhi/common/Utility.hpp"
#include "rhi/common/Resource.hpp"
#include "rhi/d3d12/D3D12Raytracing.hpp"
#include "rhi/d3d12/resource/Device.hpp"
#include "rhi/d3d12/resource/CommandList.hpp"
#include "rhi/d3d12/resource/Texture.hpp"
#include "rhi/d3d12/resource/ComputePipeline.hpp"
#include "rhi/d3d12/resource/InputLayout.hpp"
#include "rhi/d3d12/resource/BindingLayout.hpp"
#include "rhi/d3d12/resource/BindingSet.hpp"
#include "rhi/d3d12/resource/AccelStruct.hpp"
#include "rhi/d3d12/resource/OpacityMicromap.hpp"
#include "rhi/d3d12/resource/DescriptorTable.hpp"
#include <pix_win.h>

namespace slug
{
namespace rhi
{
namespace d3d12
{
Queue::Queue(const Context& context, ID3D12CommandQueue* queue)
    : queue(queue)
    , m_context(context)
{
    SLUG_ASSERT(queue);
    m_context.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

uint64_t Queue::UpdateLastCompletedInstance()
{
    if (lastCompletedInstance < lastSubmittedInstance)
    {
        lastCompletedInstance = fence->GetCompletedValue();
    }
    return lastCompletedInstance;
}

BufferChunk::~BufferChunk()
{
    if (buffer && cpuVa)
    {
        buffer->Unmap(0, nullptr);
        cpuVa = nullptr;
    }
}

UploadManager::UploadManager(const Context& context, class Queue* pQueue, size_t defaultChunkSize, uint64_t memoryLimit, bool isScratchBuffer)
    : m_context(context)
    , m_queue(pQueue)
    , m_defaultChunkSize(defaultChunkSize)
    , m_memoryLimit(memoryLimit)
    , m_isScratchBuffer(isScratchBuffer)
{
    SLUG_ASSERT(pQueue)
}

bool UploadManager::SuballocateBuffer(uint64_t size, ID3D12GraphicsCommandList* pCommandList, ID3D12Resource** pBuffer, size_t* pOffset, void** pCpuVa, D3D12_GPU_VIRTUAL_ADDRESS* pGpuVa, uint64_t currentVersion, uint32_t alignment)
{
    SLUG_ASSERT(!m_isScratchBuffer || pCommandList);

    core::TReferencePtr<BufferChunk> chunkToRetire = nullptr;

    if (m_currentChunk != nullptr)
    {
        uint64_t alignedOffset = TAlign(m_currentChunk->writePointer, (uint64_t)alignment);
        uint64_t endOfDataInChunk = alignedOffset + size;

        if (endOfDataInChunk <= m_currentChunk->bufferSize)
        {
            m_currentChunk->writePointer = endOfDataInChunk;

            if (pBuffer)
            {
                *pBuffer = m_currentChunk->buffer;
            }

            if (pOffset)
            {
                *pOffset = alignedOffset;
            }

            if (pCpuVa && m_currentChunk->cpuVa)
            {
                *pCpuVa = (char*)m_currentChunk->cpuVa + alignedOffset;
            }

            if (pGpuVa && m_currentChunk->gpuVa)
            {
                *pGpuVa = m_currentChunk->gpuVa + alignedOffset;
            }

            return true;
        }

        chunkToRetire = m_currentChunk;
        m_currentChunk.reset();
    }

    uint64_t completedInstance = m_queue->lastCompletedInstance;

    for (auto it = m_chunkPool.begin(); it != m_chunkPool.end(); ++it)
    {
        core::TReferencePtr<BufferChunk> chunk = *it;

        if (VersionGetSubmitted(chunk->version) && VersionGetInstance(chunk->version) <= completedInstance)
        {
            chunk->version = 0;
        }

        if (chunk->version == 0 && chunk->bufferSize >= size)
        {
            m_chunkPool.erase(it);
            m_currentChunk = chunk;
            break;
        }
    }

    if (chunkToRetire)
    {
        m_chunkPool.push_back(chunkToRetire);
    }

    if (!m_currentChunk)
    {
        uint64_t sizeToAllocate = TAlign(math::TMax(size, m_defaultChunkSize), BufferChunk::SIZE_ALIGNMENT);

        if ((m_memoryLimit > 0) && (m_allocatedMemory + sizeToAllocate > m_memoryLimit))
        {
            if (m_isScratchBuffer)
            {
                core::TReferencePtr<BufferChunk> bestChunk;
                for (const auto& candidateChunk : m_chunkPool)
                {
                    if (candidateChunk->bufferSize >= sizeToAllocate)
                    {
                        if (!bestChunk)
                        {
                            bestChunk = candidateChunk;
                            continue;
                        }

                        bool candidateSubmitted = VersionGetSubmitted(candidateChunk->version);
                        bool bestSubmitted = VersionGetSubmitted(bestChunk->version);
                        uint64_t candidateInstance = VersionGetInstance(candidateChunk->version);
                        uint64_t bestInstance = VersionGetInstance(bestChunk->version);

                        if (candidateSubmitted && !bestSubmitted ||
                            candidateSubmitted == bestSubmitted && candidateInstance < bestInstance ||
                            candidateSubmitted == bestSubmitted && candidateInstance == bestInstance
                            && candidateChunk->bufferSize > bestChunk->bufferSize)
                        {
                            bestChunk = candidateChunk;
                        }
                    }
                }

                if (!bestChunk)
                {
                    return false;
                }

                m_chunkPool.erase(std::find(m_chunkPool.begin(), m_chunkPool.end(), bestChunk));
                m_currentChunk = bestChunk;

                D3D12_RESOURCE_BARRIER barrier = {};
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
                barrier.UAV.pResource = bestChunk->buffer;
                pCommandList->ResourceBarrier(1, &barrier);
            }
            else
            {
                return false;
            }
        } else
        {
            m_currentChunk = CreateChunk(sizeToAllocate);
        }
    }

    m_currentChunk->version = currentVersion;
    m_currentChunk->writePointer = size;

    if (pBuffer)
    {
        *pBuffer = m_currentChunk->buffer;
    }

    if (pOffset)
    {
        *pOffset = 0;
    }

    if (pCpuVa)
    {
        *pCpuVa = m_currentChunk->cpuVa;
    }

    if (pGpuVa)
    {
        *pGpuVa = m_currentChunk->gpuVa;
    }

    return true;
}

void UploadManager::SubmitChunks(uint64_t currentVersion, uint64_t submittedVersion)
{
    if (m_currentChunk)
    {
        m_chunkPool.push_back(m_currentChunk);
        m_currentChunk.reset();
    }

    for (const auto& chunk : m_chunkPool)
    {
        if (chunk->version == currentVersion)
        {
            chunk->version = submittedVersion;
        }
    }
}

SLUG_NODISCARD core::TReferencePtr<BufferChunk> UploadManager::CreateChunk(size_t size) const
{
    core::TReferencePtr<BufferChunk> chunk = new BufferChunk();

    size = TAlign(size, BufferChunk::SIZE_ALIGNMENT);

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = m_isScratchBuffer ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = size;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    if (m_isScratchBuffer)
    {
        bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    HRESULT hr = m_context.device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        m_isScratchBuffer ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS : D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&chunk->buffer));

    if (FAILED(hr))
    {
        return nullptr;
    }

    if (!m_isScratchBuffer)
    {
        hr = chunk->buffer->Map(0, nullptr, &chunk->cpuVa);

        if (FAILED(hr))
            return nullptr;
    }

    chunk->bufferSize = size;
    chunk->gpuVa = chunk->buffer->GetGPUVirtualAddress();
    chunk->identifier = uint32_t(m_chunkPool.size());

    std::wstringstream wss;
    if (m_isScratchBuffer)
    {
        wss << L"DXR Scratch Buffer " << chunk->identifier;
    } else
    {
        wss << L"Upload Buffer " << chunk->identifier;
    }
    chunk->buffer->SetName(wss.str().c_str());
    return chunk;
}

CommandList::CommandList(class Device* device, const Context& context, DeviceResources& resources, const CommandListParameters& params)
    : m_context(context)
    , m_resources(resources)
    , m_device(device)
    , m_queue(device->GetQueue(params.queueType))
    , m_uploadManager(context, m_queue, params.uploadChunkSize, 0, false)
    , m_dxrScratchManager(context, m_queue, params.scratchChunkSize, params.scratchMaxMemory, true)
    , m_desc(params)
{
}

core::TReferencePtr<CommandListInstance> CommandList::Executed(Queue* pQueue)
{
    core::TReferencePtr<CommandListInstance> instance = m_instance;
    instance->fence = pQueue->fence;
    instance->submittedInstance = pQueue->lastSubmittedInstance;
    m_instance.reset();

    m_activeCommandList->lastSubmittedInstance = pQueue->lastSubmittedInstance;
    m_commandListPool.push_back(m_activeCommandList);
    m_activeCommandList.reset();

    for (const auto& it : instance->referencedStagingTextures)
    {
        it->lastUseFence = pQueue->fence;
        it->lastUseFenceValue = instance->submittedInstance;
    }

    for (const auto& it : instance->referencedStagingBuffers)
    {
        it->lastUseFence = pQueue->fence;
        it->lastUseFenceValue = uint32_t(instance->submittedInstance);
    }

    for (const auto& it : instance->referencedTimerQueries)
    {
        it->started = true;
        it->resolved = false;
        it->fence = pQueue->fence;
        it->fenceCounter = instance->submittedInstance;
    }

    m_stateTracker.CommandListSubmitted();

    uint64_t submittedVersion = MakeVersion(instance->submittedInstance, m_desc.queueType, true);
    m_uploadManager.SubmitChunks(m_recordingVersion, submittedVersion);
    m_dxrScratchManager.SubmitChunks(m_recordingVersion, submittedVersion);
    m_recordingVersion = 0;

    return instance;
}

void CommandList::RequireTextureState(ITexture* itexture, TextureSubresourceSet subresources, ResourceStates state)
{
    Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(itexture);
    m_stateTracker.RequireTextureState(texture, subresources, state);
}

void CommandList::RequireBufferState(IBuffer* ibuffer, ResourceStates state)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);
    m_stateTracker.RequireBufferState(buffer, state);
}

Object CommandList::GetNativeObject(ObjectType objectType)
{
    switch (objectType)
    {
    case object_types::D3D12_GraphicsCommandList:
        if (m_activeCommandList)
        {
            return Object(m_activeCommandList->commandList.get());
        }
        else
        {
            return nullptr;
        }
    case object_types::D3D12_CommandAllocator:
        if (m_activeCommandList)
        {
            return Object(m_activeCommandList->allocator.get());
        }
        else
        {
            return nullptr;
        }
    case object_types::rhi_D3D12_CommandList:
        return this;

    default:
        return nullptr;
    }
}

const CommandListParameters& CommandList::GetDesc()
{
    return m_desc;
}

CommandListState CommandList::GetCurrentState()
{
    return CommandListState::Initial;
}

ResourceStatesBitset CommandList::GetTextureSubresourceState(ITexture* itexture, ArraySlice arraySlice, MipLevel mipLevel)
{
    Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(itexture);
    return m_stateTracker.GetTextureSubresourceState(texture, arraySlice, mipLevel);

}

ResourceStatesBitset CommandList::GetBufferState(IBuffer* ibuffer)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);
    return m_stateTracker.GetBufferState(buffer);
}

ID3D12CommandList* CommandList::GetD3D12CommandList() const
{
    return m_activeCommandList->commandList.get();
}

rhi::IDevice* CommandList::GetDevice()
{
    return m_device;
}

void CommandList::Open()
{
    uint64_t completedInstance = m_queue->UpdateLastCompletedInstance();

    core::TReferencePtr<InternalCommandList> chunk;

    if (!m_commandListPool.empty())
    {
        chunk = m_commandListPool.front();

        if (chunk->lastSubmittedInstance <= completedInstance)
        {
            chunk->allocator->Reset();
            chunk->commandList->Reset(chunk->allocator, nullptr);
            m_commandListPool.pop_front();
        }
        else
        {
            chunk = nullptr;
        }
    }

    if (chunk == nullptr)
    {
        chunk = CreateInternalCommandList();
    }

    m_activeCommandList = chunk;

    m_instance = core::MakeReference<CommandListInstance>();
    m_instance->commandAllocator = m_activeCommandList->allocator;
    m_instance->commandList = m_activeCommandList->commandList;
    m_instance->commandQueue = m_desc.queueType;

    m_recordingVersion = MakeVersion(m_queue->recordingInstance++, m_desc.queueType, false);
}

void CommandList::Close()
{
    m_stateTracker.KeepBufferInitialStates();
    m_stateTracker.KeepTextureInitialStates();
    CommitBarriers();


    m_activeCommandList->commandList->Close();

    ClearStateCache();

    m_currentUploadBuffer = nullptr;
    m_volatileConstantBufferAddresses.clear();
    m_shaderTableStates.clear();
}

void CommandList::ClearState()
{
    m_activeCommandList->commandList->ClearState(nullptr);
    ClearStateCache();
    CommitDescriptorHeaps();
}

void CommandList::ClearTextureFloat(ITexture* itexture, TextureSubresourceSet subresources, const Color& clearColor)
{
    Texture* t = core::MemoryUtilities::CheckedCast<Texture*>(itexture);

#ifdef DEBUG_MODE
    const FormatInfo& formatInfo = GetFormatInfo(t->desc.format);
    SLUG_ASSERT(!formatInfo.hasDepth && !formatInfo.hasStencil);
    SLUG_ASSERT(t->desc.uav || t->desc.renderTarget);
#endif

    subresources = subresources.Resolve(t->desc, false);

    m_instance->referencedResources.push_back(t);

    if (t->desc.renderTarget)
    {
        if (m_enableAutomaticBarriers)
        {
            RequireTextureState(t, subresources, ResourceStates::RenderTarget);
        }
        CommitBarriers();

        for (MipLevel mipLevel = subresources.baseMipLevel; mipLevel < subresources.baseMipLevel + subresources.numMipLevels; mipLevel++)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE RTV = { t->GetNativeView(object_types::D3D12_RenderTargetViewDescriptor, FormatType::UNKNOWN, subresources, TextureDimension::Unknown).integer };

            m_activeCommandList->commandList->ClearRenderTargetView(RTV, &clearColor.r, 0, nullptr);
        }
    } else
    {
        if (m_enableAutomaticBarriers)
        {
            RequireTextureState(t, subresources, ResourceStates::UnorderedAccess);
        }
        CommitBarriers();
        CommitDescriptorHeaps();

        for (MipLevel mipLevel = subresources.baseMipLevel; mipLevel < subresources.baseMipLevel + subresources.numMipLevels; mipLevel++)
        {
            DescriptorIndex index = t->GetClearMipLevelUAV(mipLevel);

            SLUG_ASSERT(index != INVALID_DESCRIPTOR_INDEX);

            m_activeCommandList->commandList->ClearUnorderedAccessViewFloat(
                m_resources.shaderResourceViewHeap.GetGpuHandle(index),
                m_resources.shaderResourceViewHeap.GetCpuHandle(index),
                t->resource, &clearColor.r, 0, nullptr);
        }
    }
}

void CommandList::ClearDepthStencilTexture(ITexture* itexture, TextureSubresourceSet subresources, bool clearDepth, float depth, bool clearStencil, uint8_t stencil)
{
    if (!clearDepth && !clearStencil)
    {
        return;
    }

    Texture* t = core::MemoryUtilities::CheckedCast<Texture*>(itexture);

#ifdef DEBUG_MODE
    const FormatInfo& formatInfo = GetFormatInfo(t->desc.format);
    SLUG_ASSERT(t->desc.renderTarget);
    SLUG_ASSERT(formatInfo.hasDepth || formatInfo.hasStencil);
#endif

    subresources = subresources.Resolve(t->desc, false);

    m_instance->referencedResources.push_back(t);

    if (m_enableAutomaticBarriers)
    {
        RequireTextureState(t, subresources, ResourceStates::DepthWrite);
    }
    CommitBarriers();

    D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
    if (!clearDepth)
    {
        clearFlags = D3D12_CLEAR_FLAG_STENCIL;
    } else if (!clearStencil)
    {
        clearFlags = D3D12_CLEAR_FLAG_DEPTH;
    }

    for (MipLevel mipLevel = subresources.baseMipLevel; mipLevel < subresources.baseMipLevel + subresources.numMipLevels; mipLevel++)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE DSV = { t->GetNativeView(object_types::D3D12_DepthStencilViewDescriptor, FormatType::UNKNOWN, subresources, TextureDimension::Unknown).integer };
        m_activeCommandList->commandList->ClearDepthStencilView(DSV, clearFlags, depth, stencil, 0, nullptr);
    }
}

void CommandList::ClearTextureUint(ITexture* itexture, TextureSubresourceSet subresources, uint32_t clearColor)
{
    Texture* t = core::MemoryUtilities::CheckedCast<Texture*>(itexture);

#ifdef DEBUG_MODE
    const FormatInfo& formatInfo = GetFormatInfo(t->desc.format);
    SLUG_ASSERT(!formatInfo.hasDepth && !formatInfo.hasStencil);
    SLUG_ASSERT(t->desc.uav || t->desc.renderTarget);
#endif

    subresources = subresources.Resolve(t->desc, false);

    uint32_t clearValues[4] = { clearColor, clearColor, clearColor, clearColor };

    m_instance->referencedResources.push_back(t);

    if (t->desc.uav)
    {
        if (m_enableAutomaticBarriers)
        {
            RequireTextureState(t, subresources, ResourceStates::UnorderedAccess);
        }
        CommitBarriers();

        CommitDescriptorHeaps();

        for (MipLevel mipLevel = subresources.baseMipLevel; mipLevel < subresources.baseMipLevel + subresources.numMipLevels; mipLevel++)
        {
            DescriptorIndex index = t->GetClearMipLevelUAV(mipLevel);

            SLUG_ASSERT(index != INVALID_DESCRIPTOR_INDEX);

            m_activeCommandList->commandList->ClearUnorderedAccessViewUint(
                m_resources.shaderResourceViewHeap.GetGpuHandle(index),
                m_resources.shaderResourceViewHeap.GetCpuHandle(index),
                t->resource, clearValues, 0, nullptr);
        }
    } else if (t->desc.renderTarget)
    {
        if (m_enableAutomaticBarriers)
        {
            RequireTextureState(t, subresources, ResourceStates::RenderTarget);
        }
        CommitBarriers();

        for (MipLevel mipLevel = subresources.baseMipLevel; mipLevel < subresources.baseMipLevel + subresources.numMipLevels; mipLevel++)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE RTV = { t->GetNativeView(object_types::D3D12_RenderTargetViewDescriptor, FormatType::UNKNOWN, subresources, TextureDimension::Unknown).integer };

            float floatColor[4] = { (float)clearColor, (float)clearColor, (float)clearColor, (float)clearColor };
            m_activeCommandList->commandList->ClearRenderTargetView(RTV, floatColor, 0, nullptr);
        }
    }
}

void CommandList::CopyTexture(ITexture* idst, const TextureSlice& dstSlice, ITexture* isrc, const TextureSlice& srcSlice)
{
    Texture* dst = core::MemoryUtilities::CheckedCast<Texture*>(idst);
    Texture* src = core::MemoryUtilities::CheckedCast<Texture*>(isrc);

    auto resolvedDstSlice = dstSlice.Resolve(dst->desc);
    auto resolvedSrcSlice = srcSlice.Resolve(src->desc);

    SLUG_ASSERT(resolvedDstSlice.width == resolvedSrcSlice.width);
    SLUG_ASSERT(resolvedDstSlice.height == resolvedSrcSlice.height);

    UINT dstSubresource = D3D12Utility::CalcSubresource(resolvedDstSlice.mipLevel, resolvedDstSlice.arraySlice, 0, dst->desc.mipLevels, dst->desc.arraySize);
    UINT srcSubresource = D3D12Utility::CalcSubresource(resolvedSrcSlice.mipLevel, resolvedSrcSlice.arraySlice, 0, src->desc.mipLevels, src->desc.arraySize);

    D3D12_TEXTURE_COPY_LOCATION dstLocation;
    dstLocation.pResource = dst->resource;
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = dstSubresource;

    D3D12_TEXTURE_COPY_LOCATION srcLocation;
    srcLocation.pResource = src->resource;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.SubresourceIndex = srcSubresource;

    D3D12_BOX srcBox;
    srcBox.left = resolvedSrcSlice.x;
    srcBox.top = resolvedSrcSlice.y;
    srcBox.front = resolvedSrcSlice.z;
    srcBox.right = resolvedSrcSlice.x + resolvedSrcSlice.width;
    srcBox.bottom = resolvedSrcSlice.y + resolvedSrcSlice.height;
    srcBox.back = resolvedSrcSlice.z + resolvedSrcSlice.depth;

    if (m_enableAutomaticBarriers)
    {
        RequireTextureState(dst, TextureSubresourceSet(resolvedDstSlice.mipLevel, 1, resolvedDstSlice.arraySlice, 1), ResourceStates::CopyDest);
        RequireTextureState(src, TextureSubresourceSet(resolvedSrcSlice.mipLevel, 1, resolvedSrcSlice.arraySlice, 1), ResourceStates::CopySource);
    }
    CommitBarriers();

    m_instance->referencedResources.push_back(dst);
    m_instance->referencedResources.push_back(src);

    m_activeCommandList->commandList->CopyTextureRegion(&dstLocation,
        resolvedDstSlice.x,
        resolvedDstSlice.y,
        resolvedDstSlice.z,
        &srcLocation,
        &srcBox);
}

void CommandList::CopyTexture(IStagingTexture* idst, const TextureSlice& dstSlice, ITexture* isrc, const TextureSlice& srcSlice)
{
    Texture* src = core::MemoryUtilities::CheckedCast<Texture*>(isrc);
    StagingTexture* dst = core::MemoryUtilities::CheckedCast<StagingTexture*>(idst);

    auto resolvedDstSlice = dstSlice.Resolve(dst->desc);
    auto resolvedSrcSlice = srcSlice.Resolve(src->desc);

    UINT srcSubresource = D3D12Utility::CalcSubresource(resolvedSrcSlice.mipLevel, resolvedSrcSlice.arraySlice, 0, src->desc.mipLevels, src->desc.arraySize);

    if (m_enableAutomaticBarriers)
    {
        RequireTextureState(src, TextureSubresourceSet(resolvedSrcSlice.mipLevel, 1, resolvedSrcSlice.arraySlice, 1), ResourceStates::CopySource);
        RequireBufferState(dst->buffer, ResourceStates::CopyDest);
    }
    CommitBarriers();

    m_instance->referencedResources.push_back(src);
    m_instance->referencedStagingTextures.push_back(dst);

    auto dstRegion = dst->GetSliceRegion(m_context.device, resolvedDstSlice);

    D3D12_TEXTURE_COPY_LOCATION dstLocation;
    dstLocation.pResource = dst->buffer->resource;
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dstLocation.PlacedFootprint = dstRegion.footprint;

    D3D12_TEXTURE_COPY_LOCATION srcLocation;
    srcLocation.pResource = src->resource;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.SubresourceIndex = srcSubresource;

    D3D12_BOX srcBox;
    srcBox.left = resolvedSrcSlice.x;
    srcBox.top = resolvedSrcSlice.y;
    srcBox.front = resolvedSrcSlice.z;
    srcBox.right = resolvedSrcSlice.x + resolvedSrcSlice.width;
    srcBox.bottom = resolvedSrcSlice.y + resolvedSrcSlice.height;
    srcBox.back = resolvedSrcSlice.z + resolvedSrcSlice.depth;

    m_activeCommandList->commandList->CopyTextureRegion(&dstLocation, resolvedDstSlice.x, resolvedDstSlice.y, resolvedDstSlice.z, &srcLocation, &srcBox);
}

void CommandList::CopyTexture(ITexture* idst, const TextureSlice& dstSlice, IStagingTexture* isrc, const TextureSlice& srcSlice)
{
    StagingTexture* src = core::MemoryUtilities::CheckedCast<StagingTexture*>(isrc);
    Texture* dst = core::MemoryUtilities::CheckedCast<Texture*>(idst);

    auto resolvedDstSlice = dstSlice.Resolve(dst->desc);
    auto resolvedSrcSlice = srcSlice.Resolve(src->desc);

    UINT dstSubresource = D3D12Utility::CalcSubresource(resolvedDstSlice.mipLevel, resolvedDstSlice.arraySlice, 0, dst->desc.mipLevels, dst->desc.arraySize);

    if (m_enableAutomaticBarriers)
    {
        RequireTextureState(dst, TextureSubresourceSet(resolvedDstSlice.mipLevel, 1, resolvedDstSlice.arraySlice, 1), ResourceStates::CopyDest);
        RequireBufferState(src->buffer, ResourceStates::CopySource);
    }
    CommitBarriers();

    m_instance->referencedResources.push_back(dst);
    m_instance->referencedStagingTextures.push_back(src);

    auto srcRegion = src->GetSliceRegion(m_context.device, resolvedSrcSlice);

    D3D12_TEXTURE_COPY_LOCATION dstLocation;
    dstLocation.pResource = dst->resource;
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = dstSubresource;

    D3D12_TEXTURE_COPY_LOCATION srcLocation;
    srcLocation.pResource = src->buffer->resource;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint = srcRegion.footprint;

    D3D12_BOX srcBox;
    srcBox.left = resolvedSrcSlice.x;
    srcBox.top = resolvedSrcSlice.y;
    srcBox.front = resolvedSrcSlice.z;
    srcBox.right = resolvedSrcSlice.x + resolvedSrcSlice.width;
    srcBox.bottom = resolvedSrcSlice.y + resolvedSrcSlice.height;
    srcBox.back = resolvedSrcSlice.z + resolvedSrcSlice.depth;

    m_activeCommandList->commandList->CopyTextureRegion(&dstLocation, resolvedDstSlice.x, resolvedDstSlice.y, resolvedDstSlice.z, &srcLocation, &srcBox);
}

void CommandList::WriteTexture(ITexture* idst, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch)
{
    Texture* dst = core::MemoryUtilities::CheckedCast<Texture*>(idst);

    if (m_enableAutomaticBarriers)
    {
        RequireTextureState(dst, TextureSubresourceSet(mipLevel, 1, arraySlice, 1), ResourceStates::CopyDest);
    }
    CommitBarriers();

    uint32_t subresource = D3D12Utility::CalcSubresource(mipLevel, arraySlice, 0, dst->desc.mipLevels, dst->desc.arraySize);

    D3D12_RESOURCE_DESC resourceDesc = dst->resource->GetDesc();
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    uint32_t numRows;
    uint64_t rowSizeInBytes;
    uint64_t totalBytes;

    m_context.device->GetCopyableFootprints(&resourceDesc, subresource, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

    void* cpuVA;
    ID3D12Resource* uploadBuffer;
    size_t offsetInUploadBuffer;
    if (!m_uploadManager.SuballocateBuffer(totalBytes, nullptr, &uploadBuffer, &offsetInUploadBuffer, &cpuVA, nullptr, m_recordingVersion, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT))
    {
        SLUG_LOG(RHILogCategory, core::LogType::Error, "Couldn't suballocate an upload buffer");
        return;
    }
    footprint.Offset = uint64_t(offsetInUploadBuffer);

    SLUG_ASSERT(numRows <= footprint.Footprint.Height);

    for (uint32_t depthSlice = 0; depthSlice < footprint.Footprint.Depth; depthSlice++)
    {
        for (uint32_t row = 0; row < numRows; row++)
        {
            void* destAddress = (char*)cpuVA + footprint.Footprint.RowPitch * (row + depthSlice * numRows);
            const void* srcAddress = (const char*)data + rowPitch * row + depthPitch * depthSlice;
            memcpy(destAddress, srcAddress, math::TMin(rowPitch, rowSizeInBytes));
        }
    }

    D3D12_TEXTURE_COPY_LOCATION destCopyLocation;
    destCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    destCopyLocation.SubresourceIndex = subresource;
    destCopyLocation.pResource = dst->resource;

    D3D12_TEXTURE_COPY_LOCATION srcCopyLocation;
    srcCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcCopyLocation.PlacedFootprint = footprint;
    srcCopyLocation.pResource = uploadBuffer;

    m_instance->referencedResources.push_back(dst);

    if (uploadBuffer != m_currentUploadBuffer)
    {
        m_instance->referencedNativeResources.push_back(uploadBuffer);
        m_currentUploadBuffer = uploadBuffer;
    }

    m_activeCommandList->commandList->CopyTextureRegion(&destCopyLocation, 0, 0, 0, &srcCopyLocation, nullptr);
}

void CommandList::ResolveTexture(ITexture* idst, const TextureSubresourceSet& dstSubresources, ITexture* isrc, const TextureSubresourceSet& srcSubresources)
{
    Texture* dst = core::MemoryUtilities::CheckedCast<Texture*>(idst);
    Texture* src = core::MemoryUtilities::CheckedCast<Texture*>(isrc);

    TextureSubresourceSet dstSR = dstSubresources.Resolve(dst->desc, false);
    TextureSubresourceSet srcSR = srcSubresources.Resolve(src->desc, false);

    if (dstSR.numArraySlices != srcSR.numArraySlices || dstSR.numMipLevels != srcSR.numMipLevels)
    {
        return;
    }

    if (m_enableAutomaticBarriers)
    {
        RequireTextureState(idst, dstSubresources, ResourceStates::ResolveDest);
        RequireTextureState(isrc, srcSubresources, ResourceStates::ResolveSource);
    }
    CommitBarriers();

    const DxgiFormatMapping& formatMapping = D3D12Utility::GetDxgiFormatMapping(dst->desc.format);

    for (int plane = 0; plane < dst->planeCount; plane++)
    {
        for (ArraySlice arrayIndex = 0; arrayIndex < dstSR.numArraySlices; arrayIndex++)
        {
            for (MipLevel mipLevel = 0; mipLevel < dstSR.numMipLevels; mipLevel++)
            {
                uint32_t dstSubresource = D3D12Utility::CalcSubresource(mipLevel + dstSR.baseMipLevel, arrayIndex + dstSR.baseArraySlice, plane, dst->desc.mipLevels, dst->desc.arraySize);
                uint32_t srcSubresource = D3D12Utility::CalcSubresource(mipLevel + srcSR.baseMipLevel, arrayIndex + srcSR.baseArraySlice, plane, src->desc.mipLevels, src->desc.arraySize);
                m_activeCommandList->commandList->ResolveSubresource(dst->resource, dstSubresource, src->resource, srcSubresource, formatMapping.rtvFormat);
            }
        }
    }
}

void CommandList::WriteBuffer(IBuffer* ibuffer, const void* data, size_t dataSize, uint64_t destOffsetBytes)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);

    void* cpuVA;
    D3D12_GPU_VIRTUAL_ADDRESS gpuVA;
    ID3D12Resource* uploadBuffer;
    size_t offsetInUploadBuffer;
    if (!m_uploadManager.SuballocateBuffer(dataSize, nullptr, &uploadBuffer, &offsetInUploadBuffer, &cpuVA, &gpuVA, m_recordingVersion, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT))
    {
        SLUG_LOG(RHILogCategory, core::LogType::Error, "Couldn't suballocate an upload buffer");
        return;
    }

    if (uploadBuffer != m_currentUploadBuffer)
    {
        m_instance->referencedNativeResources.push_back(uploadBuffer);
        m_currentUploadBuffer = uploadBuffer;
    }

    memcpy(cpuVA, data, dataSize);

    if (buffer->desc.volatileBuffer)
    {
        m_volatileConstantBufferAddresses[buffer] = gpuVA;
        m_anyVolatileBufferWrites = true;
    }
    else
    {
        if (m_enableAutomaticBarriers)
        {
            RequireBufferState(buffer, ResourceStates::CopyDest);
        }
        CommitBarriers();

        m_instance->referencedResources.push_back(buffer);

        m_activeCommandList->commandList->CopyBufferRegion(buffer->resource, destOffsetBytes, uploadBuffer, offsetInUploadBuffer, dataSize);
    }
}

void CommandList::ClearBufferUInt(IBuffer* ibuffer, uint32_t clearValue)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);

    if (!buffer->desc.useUavs)
    {
        core::String str = core::StringUtility::Sprintfs("Cannot clear buffer %s, because it was created with canHaveUAVs = false", buffer->desc.debugName.c_str());
        return;
    }

    if (m_enableAutomaticBarriers)
    {
        RequireBufferState(buffer, ResourceStates::UnorderedAccess);
    }
    CommitBarriers();

    CommitDescriptorHeaps();

    DescriptorIndex clearUAV = buffer->GetClearUAV();
    SLUG_ASSERT(clearUAV != INVALID_DESCRIPTOR_INDEX);

    m_instance->referencedResources.push_back(buffer);

    const uint32_t values[4] = { clearValue, clearValue, clearValue, clearValue };
    m_activeCommandList->commandList->ClearUnorderedAccessViewUint(
        m_resources.shaderResourceViewHeap.GetGpuHandle(clearUAV),
        m_resources.shaderResourceViewHeap.GetCpuHandle(clearUAV),
        buffer->resource, values, 0, nullptr);
}

void CommandList::CopyBuffer(IBuffer* idst, uint64_t dstOffsetBytes, IBuffer* isrc, uint64_t srcOffsetBytes, uint64_t dataSizeBytes)
{
    Buffer* dst = core::MemoryUtilities::CheckedCast<Buffer*>(idst);
    Buffer* src = core::MemoryUtilities::CheckedCast<Buffer*>(isrc);

    if (m_enableAutomaticBarriers)
    {
        RequireBufferState(dst, ResourceStates::CopyDest);
        RequireBufferState(src, ResourceStates::CopySource);
    }
    CommitBarriers();

    if (src->desc.cpuAccess != CpuAccessMode::None)
    {
        m_instance->referencedStagingBuffers.push_back(src);
    }
    else
    {
        m_instance->referencedResources.push_back(src);
    }

    if (dst->desc.cpuAccess != CpuAccessMode::None)
    {
        m_instance->referencedStagingBuffers.push_back(dst);
    }
    else
    {
        m_instance->referencedResources.push_back(dst);
    }

    m_activeCommandList->commandList->CopyBufferRegion(dst->resource, dstOffsetBytes, src->resource, srcOffsetBytes, dataSizeBytes);

}

void CommandList::SetPushConstants(const void* data, size_t byteSize)
{
    const RootSignature* rootsig = nullptr;
    bool isGraphics = false;

    if (m_currentGraphicsStateValid && m_currentGraphicsState.pipeline)
    {
        GraphicsPipeline* pso = core::MemoryUtilities::CheckedCast<GraphicsPipeline*>(m_currentGraphicsState.pipeline);
        rootsig = pso->rootSignature;
        isGraphics = true;
    }
    else if (m_currentComputeStateValid && m_currentComputeState.pipeline)
    {
        ComputePipeline* pso = core::MemoryUtilities::CheckedCast<ComputePipeline*>(m_currentComputeState.pipeline);
        rootsig = pso->rootSignature;
        isGraphics = false;
    }
    else if (m_currentRayTracingStateValid && m_currentRayTracingState.shaderTable)
    {
        RayTracingPipeline* pso = core::MemoryUtilities::CheckedCast<RayTracingPipeline*>(m_currentRayTracingState.shaderTable->GetPipeline());
        rootsig = pso->globalRootSignature;
        isGraphics = false;
    }
    else if (m_currentMeshletStateValid && m_currentMeshletState.pipeline)
    {
        MeshletPipeline* pso = core::MemoryUtilities::CheckedCast<MeshletPipeline*>(m_currentMeshletState.pipeline);
        rootsig = pso->rootSignature;
        isGraphics = true;
    }

    if (!rootsig || !rootsig->pushConstantByteSize)
    {
        return;
    }

    SLUG_ASSERT(byteSize == rootsig->pushConstantByteSize);

    if (isGraphics)
    {
        m_activeCommandList->commandList->SetGraphicsRoot32BitConstants(rootsig->rootParameterPushConstants, UINT(byteSize / 4), data, 0);
    }
    else
    {
        m_activeCommandList->commandList->SetComputeRoot32BitConstants(rootsig->rootParameterPushConstants, UINT(byteSize / 4), data, 0);
    }
}

void CommandList::SetGraphicsState(const GraphicsState& state)
{
    GraphicsPipeline* pso = core::MemoryUtilities::CheckedCast<GraphicsPipeline*>(state.pipeline);
    FrameBuffer* framebuffer = core::MemoryUtilities::CheckedCast<FrameBuffer*>(state.framebuffer);

    const bool updateFramebuffer = !m_currentGraphicsStateValid || m_currentGraphicsState.framebuffer != state.framebuffer;
    const bool updateRootSignature = !m_currentGraphicsStateValid || m_currentGraphicsState.pipeline == nullptr ||
        core::MemoryUtilities::CheckedCast<GraphicsPipeline*>(m_currentGraphicsState.pipeline)->rootSignature != pso->rootSignature;

    const bool updatePipeline = !m_currentGraphicsStateValid || m_currentGraphicsState.pipeline != state.pipeline;
    const bool updateIndirectParams = !m_currentGraphicsStateValid || m_currentGraphicsState.indirectParams != state.indirectParams;

    const bool updateViewports = !m_currentGraphicsStateValid ||
        TArraysAreDifferent(m_currentGraphicsState.viewport.viewports, state.viewport.viewports) ||
        TArraysAreDifferent(m_currentGraphicsState.viewport.scissorRects, state.viewport.scissorRects);

    const bool updateBlendFactor = !m_currentGraphicsStateValid || m_currentGraphicsState.blendConstantColor != state.blendConstantColor;

    const bool updateIndexBuffer = !m_currentGraphicsStateValid || m_currentGraphicsState.indexBuffer != state.indexBuffer;
    const bool updateVertexBuffers = !m_currentGraphicsStateValid || TArraysAreDifferent(m_currentGraphicsState.vertexBuffers, state.vertexBuffers);

    const bool updateShadingRate = !m_currentGraphicsStateValid || m_currentGraphicsState.shadingRateState != state.shadingRateState;

    uint32_t binding_update_mask = 0;
    if (!m_currentGraphicsStateValid || updateRootSignature)
    {
        binding_update_mask = ~0u;
    }

    if (CommitDescriptorHeaps())
    {
        binding_update_mask = ~0u;
    }

    if (binding_update_mask == 0)
    {
        binding_update_mask = ArrayDifferenceMask(m_currentGraphicsState.bindings, state.bindings);
    }

    if (updatePipeline)
    {
        BindGraphicsPipeline(pso, updateRootSignature);
        m_instance->referencedResources.push_back(pso);
    }

    if (pso->requiresBlendFactor && updateBlendFactor)
    {
        m_activeCommandList->commandList->OMSetBlendFactor(&state.blendConstantColor.r);
    }

    if (updateFramebuffer)
    {
        BindFramebuffer(framebuffer);
        m_instance->referencedResources.push_back(framebuffer);
    }

    SetGraphicsBindings(state.bindings, binding_update_mask, state.indirectParams, updateIndirectParams, pso->rootSignature);

    if (updateIndexBuffer)
    {
        D3D12_INDEX_BUFFER_VIEW IBV = {};

        if (state.indexBuffer.buffer)
        {
            Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(state.indexBuffer.buffer);

            if (m_enableAutomaticBarriers)
            {
                RequireBufferState(buffer, ResourceStates::IndexBuffer);
            }

            IBV.Format = D3D12Utility::GetDxgiFormatMapping(state.indexBuffer.format).srvFormat;
            IBV.SizeInBytes = (UINT)(buffer->desc.byteSize - state.indexBuffer.offset);
            IBV.BufferLocation = buffer->gpuVirtualAddress + state.indexBuffer.offset;

            m_instance->referencedResources.push_back(state.indexBuffer.buffer);
        }

        m_activeCommandList->commandList->IASetIndexBuffer(&IBV);
    }

    if (updateVertexBuffers)
    {
        D3D12_VERTEX_BUFFER_VIEW VBVs[16] = {};

        InputLayout* inputLayout = core::MemoryUtilities::CheckedCast<InputLayout*>(pso->desc.inputLayout.get());

        for (size_t i = 0; i < state.vertexBuffers.size(); i++)
        {
            const VertexBufferBinding& binding = state.vertexBuffers[i];

            Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(binding.buffer);

            if (m_enableAutomaticBarriers)
            {
                RequireBufferState(buffer, ResourceStates::VertexBuffer);
            }

            auto stride_in_bytes_itr = inputLayout->elementStrides.find(binding.slot);
            if (stride_in_bytes_itr != inputLayout->elementStrides.end())
            {
                VBVs[binding.slot].StrideInBytes = static_cast<uint32_t>(stride_in_bytes_itr->second);
            }
            VBVs[binding.slot].SizeInBytes = (uint32_t)(math::TMin(buffer->desc.byteSize - binding.offset, (uint64_t)ULONG_MAX));
            VBVs[binding.slot].BufferLocation = buffer->gpuVirtualAddress + binding.offset;

            m_instance->referencedResources.push_back(buffer);
        }

        uint32_t numVertexBuffers = uint32_t(state.vertexBuffers.size());
        if (m_currentGraphicsStateValid)
        {
            numVertexBuffers = math::TMax(numVertexBuffers, uint32_t(m_currentGraphicsState.vertexBuffers.size()));
        }
        for (uint32_t i = 0; i < numVertexBuffers; i++)
        {
            m_activeCommandList->commandList->IASetVertexBuffers(i, 1, VBVs[i].BufferLocation != 0 ? &VBVs[i] : nullptr);
        }
    }

    if (updateShadingRate || updateFramebuffer)
    {
        auto framebufferDesc = framebuffer->GetDesc();
        bool shouldEnableVariableRateShading = framebufferDesc.shadingRateAttachment.Valid() && state.shadingRateState.enabled;
        bool variableRateShadingCurrentlyEnabled = m_currentGraphicsStateValid
            && m_currentGraphicsState.framebuffer->GetDesc().shadingRateAttachment.Valid() && m_currentGraphicsState.shadingRateState.enabled;

        if (shouldEnableVariableRateShading)
        {
            SetTextureState(framebufferDesc.shadingRateAttachment.texture, rhi::TextureSubresourceSet(0, 1, 0, 1), rhi::ResourceStates::ShadingRateSurface);
            Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(framebufferDesc.shadingRateAttachment.texture);
            m_activeCommandList->commandList6->RSSetShadingRateImage(texture->resource);
        } else if (variableRateShadingCurrentlyEnabled)
        {
            m_activeCommandList->commandList6->RSSetShadingRateImage(nullptr);
        }
    }

    if (updateShadingRate)
    {
        if (state.shadingRateState.enabled)
        {
            static_assert(D3D12_RS_SET_SHADING_RATE_COMBINER_COUNT == 2);
            D3D12_SHADING_RATE_COMBINER combiners[D3D12_RS_SET_SHADING_RATE_COMBINER_COUNT];
            combiners[0] = D3D12Utility::ConvertShadingRateCombiner(state.shadingRateState.pipelinePrimitiveCombiner);
            combiners[1] = D3D12Utility::ConvertShadingRateCombiner(state.shadingRateState.imageCombiner);
            m_activeCommandList->commandList6->RSSetShadingRate(D3D12Utility::ConvertPixelShadingRate(state.shadingRateState.shadingRate), combiners);
        } else if (m_currentGraphicsStateValid && m_currentGraphicsState.shadingRateState.enabled)
        {
            m_activeCommandList->commandList6->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr);
        }
    }

    CommitBarriers();

    if (updateViewports)
    {
        DX12ViewportState vpState = D3D12Utility::ConvertViewportState(pso->desc.renderState.rasterState, framebuffer->frameBufferInfo, state.viewport);

        if (vpState.numViewports)
        {
            m_activeCommandList->commandList->RSSetViewports(vpState.numViewports, vpState.viewports);
        }

        if (vpState.numScissorRects)
        {
            m_activeCommandList->commandList->RSSetScissorRects(vpState.numScissorRects, vpState.scissorRects);
        }
    }

    m_currentGraphicsStateValid = true;
    m_currentComputeStateValid = false;
    m_currentMeshletStateValid = false;
    m_currentRayTracingStateValid = false;
    m_currentGraphicsState = state;
}

void CommandList::Draw(const DrawArguments& args)
{
    UpdateGraphicsVolatileBuffers();
    m_activeCommandList->commandList->DrawInstanced(args.vertexCount, args.instanceCount, args.startVertexLocation, args.startInstanceLocation);
}

void CommandList::DrawIndexed(const DrawArguments& args)
{
    UpdateGraphicsVolatileBuffers();
    m_activeCommandList->commandList->DrawIndexedInstanced(args.vertexCount, args.instanceCount, args.startIndexLocation, args.startVertexLocation, args.startInstanceLocation);
}

void CommandList::DrawIndirect(uint32_t offsetBytes, uint32_t drawCount)
{
    Buffer* indirectParams = core::MemoryUtilities::CheckedCast<Buffer*>(m_currentGraphicsState.indirectParams);
    SLUG_ASSERT(indirectParams);
    UpdateGraphicsVolatileBuffers();
    m_activeCommandList->commandList->ExecuteIndirect(m_context.drawIndirectSignature, drawCount, indirectParams->resource, offsetBytes, nullptr, 0);

}

void CommandList::DrawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount)
{
    Buffer* indirectParams = core::MemoryUtilities::CheckedCast<Buffer*>(m_currentGraphicsState.indirectParams);
    SLUG_ASSERT(indirectParams);

    UpdateGraphicsVolatileBuffers();

    m_activeCommandList->commandList->ExecuteIndirect(m_context.drawIndexedIndirectSignature, drawCount, indirectParams->resource, offsetBytes, nullptr, 0);
}

void CommandList::SetComputeState(const ComputeState& state)
{
    ComputePipeline* pso = core::MemoryUtilities::CheckedCast<ComputePipeline*>(state.pipeline);

    const bool updateRootSignature = !m_currentComputeStateValid || m_currentComputeState.pipeline == nullptr ||
        core::MemoryUtilities::CheckedCast<ComputePipeline*>(m_currentComputeState.pipeline)->rootSignature != pso->rootSignature;

    bool updatePipeline = !m_currentComputeStateValid || m_currentComputeState.pipeline != state.pipeline;
    bool updateIndirectParams = !m_currentComputeStateValid || m_currentComputeState.indirectParams != state.indirectParams;

    uint32_t bindingUpdateMask = 0;
    if (!m_currentComputeStateValid || updateRootSignature)
    {
        bindingUpdateMask = ~0u;
    }

    if (CommitDescriptorHeaps())
    {
        bindingUpdateMask = ~0u;
    }

    if (bindingUpdateMask == 0)
    {
        bindingUpdateMask = ArrayDifferenceMask(m_currentComputeState.bindings, state.bindings);
    }

    if (updateRootSignature)
    {
        m_activeCommandList->commandList->SetComputeRootSignature(pso->rootSignature->handle);
    }

    if (updatePipeline)
    {
        m_activeCommandList->commandList->SetPipelineState(pso->pipelineState);

        m_instance->referencedResources.push_back(pso);
    }

    SetComputeBindings(state.bindings, bindingUpdateMask, state.indirectParams, updateIndirectParams, pso->rootSignature);

    UnbindShadingRateState();

    m_currentGraphicsStateValid = false;
    m_currentComputeStateValid = true;
    m_currentMeshletStateValid = false;
    m_currentRayTracingStateValid = false;
    m_currentComputeState = state;

    CommitBarriers();
}

void CommandList::Dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
{
    UpdateComputeVolatileBuffers();
    m_activeCommandList->commandList->Dispatch(groupsX, groupsY, groupsZ);
}

void CommandList::DispatchIndirect(uint32_t offsetBytes)
{
    Buffer* indirectParams = core::MemoryUtilities::CheckedCast<Buffer*>(m_currentComputeState.indirectParams);
    SLUG_ASSERT(indirectParams);
    UpdateComputeVolatileBuffers();
    m_activeCommandList->commandList->ExecuteIndirect(m_context.dispatchIndirectSignature, 1, indirectParams->resource, offsetBytes, nullptr, 0);
}

void CommandList::SetMeshletState(const MeshletState& state)
{
    MeshletPipeline* pso = core::MemoryUtilities::CheckedCast<MeshletPipeline*>(state.pipeline);
    FrameBuffer* framebuffer = core::MemoryUtilities::CheckedCast<FrameBuffer*>(state.framebuffer);

    UnbindShadingRateState();

    const bool updateFramebuffer = !m_currentMeshletStateValid || m_currentMeshletState.framebuffer != state.framebuffer;
    const bool updateRootSignature = !m_currentMeshletStateValid || m_currentMeshletState.pipeline == nullptr ||
        core::MemoryUtilities::CheckedCast<MeshletPipeline*>(m_currentMeshletState.pipeline)->rootSignature != pso->rootSignature;

    const bool update_pipeline = !m_currentMeshletStateValid || m_currentMeshletState.pipeline != state.pipeline;
    const bool update_indirect_params = !m_currentMeshletStateValid || m_currentMeshletState.indirectParams != state.indirectParams;

    const bool updateViewports = !m_currentMeshletStateValid ||
        TArraysAreDifferent(m_currentMeshletState.viewport.viewports, state.viewport.viewports) ||
        TArraysAreDifferent(m_currentMeshletState.viewport.scissorRects, state.viewport.scissorRects);

    const bool updateBlendFactor = !m_currentGraphicsStateValid || m_currentGraphicsState.blendConstantColor != state.blendConstantColor;

    uint32_t bindingUpdateMask = 0;
    if (!m_currentMeshletStateValid || updateRootSignature)
    {
        bindingUpdateMask = ~0u;
    }

    if (CommitDescriptorHeaps())
    {
        bindingUpdateMask = ~0u;
    }

    if (bindingUpdateMask == 0)
    {
        bindingUpdateMask = ArrayDifferenceMask(m_currentMeshletState.bindings, state.bindings);
    }

    if (update_pipeline)
    {
        BindMeshletPipeline(pso, updateRootSignature);
        m_instance->referencedResources.push_back(pso);
    }

    if (pso->requiresBlendFactor && updateBlendFactor)
    {
        m_activeCommandList->commandList->OMSetBlendFactor(&state.blendConstantColor.r);
    }

    if (updateFramebuffer)
    {
        BindFramebuffer(framebuffer);
        m_instance->referencedResources.push_back(framebuffer);
    }

    SetGraphicsBindings(state.bindings, bindingUpdateMask, state.indirectParams, update_indirect_params, pso->rootSignature);

    CommitBarriers();

    if (updateViewports)
    {
        DX12ViewportState vpState = D3D12Utility::ConvertViewportState(pso->desc.renderState.rasterState, framebuffer->frameBufferInfo, state.viewport);

        if (vpState.numViewports)
        {
            SLUG_ASSERT(pso->viewportState.numViewports == 0);
            m_activeCommandList->commandList->RSSetViewports(vpState.numViewports, vpState.viewports);
        }

        if (vpState.numScissorRects)
        {
            SLUG_ASSERT(pso->viewportState.numScissorRects == 0);
            m_activeCommandList->commandList->RSSetScissorRects(vpState.numScissorRects, vpState.scissorRects);
        }
    }

    m_currentGraphicsStateValid = false;
    m_currentComputeStateValid = false;
    m_currentMeshletStateValid = true;
    m_currentRayTracingStateValid = false;
    m_currentMeshletState = state;
}

void CommandList::DispatchMesh(uint32_t groupsX, uint32_t groupsY , uint32_t groupsZ)
{
    UpdateGraphicsVolatileBuffers();
    m_activeCommandList->commandList6->DispatchMesh(groupsX, groupsY, groupsZ);
}

void CommandList::SetRayTracingState(const rt::State& state)
{
    ShaderTable* shaderTable = core::MemoryUtilities::CheckedCast<ShaderTable*>(state.shaderTable);
    RayTracingPipeline* pso = shaderTable->pipeline;

    ShaderTableState* shaderTableState = GetShaderTableStateTracking(shaderTable);

    bool rebuildShaderTable = shaderTableState->committed_version != shaderTable->version ||
        shaderTableState->descriptor_heap_srv != m_resources.shaderResourceViewHeap.GetShaderVisibleHeap() ||
        shaderTableState->descriptor_heap_samplers != m_resources.samplerHeap.GetShaderVisibleHeap();

    if (rebuildShaderTable)
    {
        uint32_t entrySize = pso->GetShaderTableEntrySize();
        uint32_t sbtSize = shaderTable->GetNumEntries() * entrySize;

        unsigned char* cpuVA;
        D3D12_GPU_VIRTUAL_ADDRESS gpuVA;
        if (!m_uploadManager.SuballocateBuffer(sbtSize, nullptr, nullptr, nullptr, (void**)&cpuVA, &gpuVA, m_recordingVersion, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT))
        {
            SLUG_LOG(RHILogCategory, core::LogType::Error, "Couldn't suballocate an upload buffer");
            return;
        }

        uint32_t entryIndex = 0;

        auto writeEntry = [this, entrySize, &cpuVA, &gpuVA, &entryIndex](const ShaderTable::Entry& entry)
            {
                memcpy(cpuVA, entry.pShaderIdentifier, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

                if (entry.localBindings)
                {
                    d3d12::BindingSet* bindingSet = core::MemoryUtilities::CheckedCast<d3d12::BindingSet*>(entry.localBindings.get());
                    d3d12::BindingLayout* layout = bindingSet->layout;

                    if (layout->descriptorTableSizeSamplers > 0)
                    {
                        auto pTable = reinterpret_cast<D3D12_GPU_DESCRIPTOR_HANDLE*>(cpuVA + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + layout->rootParameterSamplers * sizeof(D3D12_GPU_DESCRIPTOR_HANDLE));
                        *pTable = m_resources.samplerHeap.GetGpuHandle(bindingSet->descriptorTableSamplers);
                    }

                    if (layout->descriptorTableSizeSrvEtc > 0)
                    {
                        auto pTable = reinterpret_cast<D3D12_GPU_DESCRIPTOR_HANDLE*>(cpuVA + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + layout->rootParameterSrvEtc * sizeof(D3D12_GPU_DESCRIPTOR_HANDLE));
                        *pTable = m_resources.shaderResourceViewHeap.GetGpuHandle(bindingSet->descriptorTableSrvEtc);
                    }

                    if (!layout->rootParametersVolatileCb.empty())
                    {
                        SLUG_LOG(RHILogCategory, core::LogType::Error, "Cannot use Volatile CBs in a shader binding table");
                        return;
                    }
                }

                cpuVA += entrySize;
                gpuVA += entrySize;
                entryIndex += 1;
            };

        D3D12_DISPATCH_RAYS_DESC& drd = shaderTableState->dispatch_rays_template;
        memset(&drd, 0, sizeof(D3D12_DISPATCH_RAYS_DESC));

        drd.RayGenerationShaderRecord.StartAddress = gpuVA;
        drd.RayGenerationShaderRecord.SizeInBytes = entrySize;
        writeEntry(shaderTable->rayGenerationShader);

        if (!shaderTable->missShaders.empty())
        {
            drd.MissShaderTable.StartAddress = gpuVA;
            drd.MissShaderTable.StrideInBytes = (shaderTable->missShaders.size() == 1) ? 0 : entrySize;
            drd.MissShaderTable.SizeInBytes = uint32_t(shaderTable->missShaders.size()) * entrySize;

            for (auto& entry : shaderTable->missShaders)
            {
                writeEntry(entry);
            }
        }

        if (!shaderTable->hitGroups.empty())
        {
            drd.HitGroupTable.StartAddress = gpuVA;
            drd.HitGroupTable.StrideInBytes = (shaderTable->hitGroups.size() == 1) ? 0 : entrySize;
            drd.HitGroupTable.SizeInBytes = uint32_t(shaderTable->hitGroups.size()) * entrySize;

            for (auto& entry : shaderTable->hitGroups)
            {
                writeEntry(entry);
            }
        }

        if (!shaderTable->callableShaders.empty())
        {
            drd.CallableShaderTable.StartAddress = gpuVA;
            drd.CallableShaderTable.StrideInBytes = (shaderTable->callableShaders.size() == 1) ? 0 : entrySize;
            drd.CallableShaderTable.SizeInBytes = uint32_t(shaderTable->callableShaders.size()) * entrySize;

            for (auto& entry : shaderTable->callableShaders)
            {
                writeEntry(entry);
            }
        }

        shaderTableState->committed_version = shaderTable->version;
        shaderTableState->descriptor_heap_srv = m_resources.shaderResourceViewHeap.GetShaderVisibleHeap();
        shaderTableState->descriptor_heap_samplers = m_resources.samplerHeap.GetShaderVisibleHeap();

        m_instance->referencedResources.push_back(shaderTable);
    }
}

void CommandList::DispatchRays(const rt::DispatchRaysArguments & args)
{
    UpdateComputeVolatileBuffers();

    if (!m_currentRayTracingStateValid)
    {
        SLUG_LOG(RHILogCategory, core::LogType::Error, "setRayTracingState must be called before dispatchRays")
        return;
    }

    ShaderTableState* shaderTableState = GetShaderTableStateTracking(m_currentRayTracingState.shaderTable);

    D3D12_DISPATCH_RAYS_DESC desc = shaderTableState->dispatch_rays_template;
    desc.Width = args.width;
    desc.Height = args.height;
    desc.Depth = args.depth;

    m_activeCommandList->commandList4->DispatchRays(&desc);
}

void CommandList::BuildOpacityMicromap(rt::IOpacityMicromap* omm, const rt::OpacityMicromapDesc& desc)
{
    SLUG_LOG(RHILogCategory, core::LogType::Error, "NotSupported");
}

void CommandList::BuildBottomLevelAccelStruct(rt::IAccelStruct* ias, const rt::GeometryDesc* pGeometries, size_t numGeometries, rt::AccelStructBuildFlagsBitset buildFlags)
{
    AccelStruct* as = core::MemoryUtilities::CheckedCast<AccelStruct*>(ias);

    const bool performUpdate = (buildFlags.Test(rt::AccelStructBuildFlags::PerformUpdate));
    if (performUpdate)
    {
        SLUG_ASSERT(as->allowUpdate);
    }

    for (uint32_t i = 0; i < numGeometries; i++)
    {
        const auto& geometryDesc = pGeometries[i];
        if (geometryDesc.geometryType == rt::GeometryType::Triangles)
        {
            const auto& triangles = geometryDesc.geometryData.triangles;

            OpacityMicromap* om = triangles.opacityMicromap ? core::MemoryUtilities::CheckedCast<OpacityMicromap*>(triangles.opacityMicromap) : nullptr;

            if (m_enableAutomaticBarriers)
            {
                RequireBufferState(triangles.indexBuffer, ResourceStates::AccelStructBuildInput);
                RequireBufferState(triangles.vertexBuffer, ResourceStates::AccelStructBuildInput);
                if (om)
                {
                    RequireBufferState(om->dataBuffer, ResourceStates::AccelStructBuildInput);
                }
                if (triangles.ommIndexBuffer)
                {
                    RequireBufferState(triangles.ommIndexBuffer, ResourceStates::AccelStructBuildInput);
                }
            }

            m_instance->referencedResources.push_back(triangles.indexBuffer);
            m_instance->referencedResources.push_back(triangles.vertexBuffer);
            if (om && om->desc.trackLiveness)
            {
                m_instance->referencedResources.push_back(om);
            }
            if (triangles.ommIndexBuffer)
            {
                m_instance->referencedResources.push_back(triangles.ommIndexBuffer);
            }
        }
        else
        {
            const auto& aabbs = geometryDesc.geometryData.aabbs;

            if (m_enableAutomaticBarriers)
            {
                RequireBufferState(aabbs.buffer, ResourceStates::AccelStructBuildInput);
            }

            m_instance->referencedResources.push_back(aabbs.buffer);
        }
    }

    CommitBarriers();

    D3D12BuildRaytracingAccelerationStructureInputs inputs;
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS asFlag = D3D12Utility::ConvertAcceslerationStructureBuildFlags(buildFlags);

    inputs.SetType(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL);
    if (as->allowUpdate)
    {
        inputs.SetFlags(asFlag | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE);
    }
    else
    {
        inputs.SetFlags(asFlag);
    }

    inputs.SetGeometryDescCount((UINT)numGeometries);
    for (uint32_t i = 0; i < numGeometries; i++)
    {
        const auto& geometryDesc = pGeometries[i];

        D3D12_GPU_VIRTUAL_ADDRESS gpuVA = 0;
        if (geometryDesc.useTransform)
        {
            void* cpuVA = nullptr;
            if (!m_uploadManager.SuballocateBuffer(sizeof(rt::AffineTransform), nullptr, nullptr, nullptr, &cpuVA, &gpuVA, m_recordingVersion, D3D12_RAYTRACING_TRANSFORM3X4_BYTE_ALIGNMENT))
            {
                SLUG_LOG(RHILogCategory, core::LogType::Error, "Couldn't suballocate an upload buffer");
                return;
            }

            memcpy(cpuVA, &geometryDesc.transform, sizeof(rt::AffineTransform));
        }

        D3D12RaytracingGeometryDesc& geomDesc = inputs.GetGeometryDesc(i);
        D3D12Raytracing::FillD3dGeometryDesc(geomDesc, geometryDesc, gpuVA);
    }

#if NVRHI_WITH_RTXMU
    core::TVector<uint64_t> accelStructsToBuild;
    core::TVector<D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS> buildInputs;
    buildInputs.push_back(inputs.GetAs<D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS>());

    if (as->rtxmuId == ~0ull)
    {
        m_context.rtxMemUtil->PopulateBuildCommandList(m_activeCommandList->commandList4.get(),
            buildInputs.data(),
            buildInputs.size(),
            accelStructsToBuild);

        as->rtxmuId = accelStructsToBuild[0];

        as->rtxmuGpuVA = m_context.rtxMemUtil->GetAccelStructGPUVA(as->rtxmuId);

        m_instance->rtxmuBuildIds.push_back(as->rtxmuId);

    }
    else
    {
        core::TVector<uint64_t> buildsToUpdate(1, as->rtxmuId);

        m_context.rtxMemUtil->PopulateUpdateCommandList(m_activeCommandList->commandList4.get(),
            buildInputs.data(),
            uint32_t(buildInputs.size()),
            buildsToUpdate);
    }
#else
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO ASPreBuildInfo = {};

    if (!core::MemoryUtilities::CheckedCast<d3d12::Device*>(m_device)->GetAccelStructPreBuildInfo(ASPreBuildInfo, as->GetDesc()))
    {
        return;
    }

    if (ASPreBuildInfo.ResultDataMaxSizeInBytes > as->dataBuffer->desc.byteSize)
    {
        core::String str = core::StringUtility::Sprintfs("BLAS %s build requires at least %ld, bytes", as->desc.debugName.c_str(), ASPreBuildInfo.ResultDataMaxSizeInBytes);
        str += core::StringUtility::Sprintfs(" in the data buffer, while the allocated buffer is only %ld bytes", as->dataBuffer->desc.byteSize);
        SLUG_LOG(RHILogCategory, core::LogType::Error, str.c_str());
        return;
    }

    uint64_t scratchSize = performUpdate ? ASPreBuildInfo.UpdateScratchDataSizeInBytes : ASPreBuildInfo.ScratchDataSizeInBytes;
    D3D12_GPU_VIRTUAL_ADDRESS scratchGpuVA = 0;
    if (!m_dxrScratchManager.SuballocateBuffer(scratchSize, m_activeCommandList->commandList, nullptr, nullptr, nullptr, &scratchGpuVA, m_recordingVersion, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT))
    {
        core::String str = core::StringUtility::Sprintfs("Couldn't suballocate a scratch buffer for BLAS %s The build requires %d bytes of scratch space.", as->desc.debugName.c_str(), scratchSize);
        SLUG_LOG(RHILogCategory, core::LogType::Error, str.c_str());
        return;
    }

    if (m_enableAutomaticBarriers)
    {
        RequireBufferState(as->dataBuffer, rhi::ResourceStates::AccelStructWrite);
    }
    CommitBarriers();
    {
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
        buildDesc.Inputs = inputs.GetAs();
        buildDesc.ScratchAccelerationStructureData = scratchGpuVA;
        buildDesc.DestAccelerationStructureData = as->dataBuffer->gpuVirtualAddress;
        buildDesc.SourceAccelerationStructureData = performUpdate ? as->dataBuffer->gpuVirtualAddress : 0;
        m_activeCommandList->commandList4->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
    }

#endif // NVRHI_WITH_RTXMU

    if (as->desc.trackLiveness)
    {
        m_instance->referencedResources.push_back(as);
    }
}

void CommandList::CompactBottomLevelAccelStructs()
{
    SLUG_LOG(RHILogCategory, core::LogType::Error, "Not Supported");
}

void CommandList::BuildTopLevelAccelStruct(rt::IAccelStruct* ias, const rt::InstanceDesc* pInstances, size_t numInstances, rt::AccelStructBuildFlagsBitset buildFlags)
{
    AccelStruct* as = core::MemoryUtilities::CheckedCast<AccelStruct*>(ias);

    as->bottomLevelAses.clear();

    as->dxrInstances.resize(numInstances);

    for (uint32_t i = 0; i < numInstances; i++)
    {
        const rt::InstanceDesc& instance = pInstances[i];
        D3D12_RAYTRACING_INSTANCE_DESC& dxrInstance = as->dxrInstances[i];

        if (instance.bottomLevelAs)
        {
            AccelStruct* blas = core::MemoryUtilities::CheckedCast<AccelStruct*>(instance.bottomLevelAs);

            if (blas->desc.trackLiveness)
            {
                as->bottomLevelAses.push_back(blas);
            }

            SLUG_STATIC_ASSERT_MSG(sizeof(dxrInstance) == sizeof(instance), "");
            memcpy(&dxrInstance, &instance, sizeof(instance));

            dxrInstance.AccelerationStructure = blas->dataBuffer->gpuVirtualAddress;

            if (m_enableAutomaticBarriers)
            {
                RequireBufferState(blas->dataBuffer, rhi::ResourceStates::AccelStructBuildBlas);
            }
        }
        else
        {
            dxrInstance.AccelerationStructure = 0;
        }
    }

    D3D12_RAYTRACING_INSTANCE_DESC* cpuVA = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpuVA = 0;
    size_t uploadsize_t = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * as->dxrInstances.size();
    if (!m_uploadManager.SuballocateBuffer(uploadsize_t, nullptr, nullptr, nullptr, (void**)&cpuVA, &gpuVA, m_recordingVersion, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT))
    {
        SLUG_LOG(RHILogCategory, core::LogType::Error, "Couldn't suballocate an upload buffer");
        return;
    }

    memcpy(cpuVA, as->dxrInstances.data(), sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * as->dxrInstances.size());

    if (m_enableAutomaticBarriers)
    {
        RequireBufferState(as->dataBuffer, rhi::ResourceStates::AccelStructWrite);
    }
    CommitBarriers();

    BuildTopLevelAccelStructInternal(as, gpuVA, numInstances, buildFlags);

    if (as->desc.trackLiveness)
    {
        m_instance->referencedResources.push_back(as);
    }

}

void CommandList::BuildTopLevelAccelStructFromBuffer(rt::IAccelStruct* ias, rhi::IBuffer* instanceBuffer, uint64_t instanceBufferOffset, size_t numInstances, rt::AccelStructBuildFlagsBitset buildFlags)
{
    AccelStruct* as = core::MemoryUtilities::CheckedCast<AccelStruct*>(ias);

    as->bottomLevelAses.clear();
    as->dxrInstances.clear();

    if (m_enableAutomaticBarriers)
    {
        RequireBufferState(as->dataBuffer, rhi::ResourceStates::AccelStructWrite);
        RequireBufferState(instanceBuffer, rhi::ResourceStates::AccelStructBuildInput);
    }
    CommitBarriers();

    BuildTopLevelAccelStructInternal(as, GetBufferGpuVA(instanceBuffer) + instanceBufferOffset, numInstances, buildFlags);

    if (as->desc.trackLiveness)
    {
        m_instance->referencedResources.push_back(as);
    }
}

void CommandList::BeginTimerQuery(ITimerQuery* iquery)
{
    TimerQuery* query = core::MemoryUtilities::CheckedCast<TimerQuery*>(iquery);
    m_instance->referencedTimerQueries.push_back(query);
    m_activeCommandList->commandList->EndQuery(m_context.timerQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, query->beginQueryIndex);
}

void CommandList::EndTimerQuery(ITimerQuery* iquery)
{
    TimerQuery* query = core::MemoryUtilities::CheckedCast<TimerQuery*>(iquery);
    m_instance->referencedTimerQueries.push_back(query);
    m_activeCommandList->commandList->EndQuery(m_context.timerQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, query->endQueryIndex);
    m_activeCommandList->commandList->ResolveQueryData(m_context.timerQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, query->beginQueryIndex, 2, m_context.timerQueryResolveBuffer->resource, query->beginQueryIndex * 8);
}

void CommandList::BeginMarker(core::StringView name)
{
    PIXBeginEvent(m_activeCommandList->commandList, 0, name.data());
}

void CommandList::EndMarker()
{
    PIXEndEvent(m_activeCommandList->commandList);
}

void CommandList::SetEnableAutomaticBarriers(bool enable)
{
    m_enableAutomaticBarriers = enable;
}

void CommandList::SetResourceStatesForBindingSet(IBindingSet* ibindingSet)
{
    if (ibindingSet->GetDesc() == nullptr)
    {
        return;
    }

    BindingSet* bindingSet = core::MemoryUtilities::CheckedCast<BindingSet*>(ibindingSet);

    for (auto bindingIndex : bindingSet->bindingsThatNeedTransitions)
    {
        const BindingSetItem& binding = bindingSet->desc.bindings[bindingIndex];

        switch (binding.type)
        {
        case ResourceType::Texture_SRV:
            RequireTextureState(core::MemoryUtilities::CheckedCast<ITexture*>(binding.resourcePtr), binding.subresources, ResourceStates::ShaderResource);
            break;

        case ResourceType::Texture_UAV:
            RequireTextureState(core::MemoryUtilities::CheckedCast<ITexture*>(binding.resourcePtr), binding.subresources, ResourceStates::UnorderedAccess);
            break;

        case ResourceType::TypedBuffer_SRV:
        case ResourceType::StructuredBuffer_SRV:
        case ResourceType::RawBuffer_SRV:
            RequireBufferState(core::MemoryUtilities::CheckedCast<IBuffer*>(binding.resourcePtr), ResourceStates::ShaderResource);
            break;

        case ResourceType::TypedBuffer_UAV:
        case ResourceType::StructuredBuffer_UAV:
        case ResourceType::RawBuffer_UAV:
            RequireBufferState(core::MemoryUtilities::CheckedCast<IBuffer*>(binding.resourcePtr), ResourceStates::UnorderedAccess);
            break;

        case ResourceType::ConstantBuffer:
            RequireBufferState(core::MemoryUtilities::CheckedCast<IBuffer*>(binding.resourcePtr), ResourceStates::ConstantBuffer);
            break;

        case ResourceType::RayTracingAccelStruct:
            RequireBufferState(core::MemoryUtilities::CheckedCast<AccelStruct*>(binding.resourcePtr)->dataBuffer, ResourceStates::AccelStructRead);
            break;

        default:
            break;
        }
    }
}

void CommandList::SetEnableUavBarriersForTexture(ITexture* itexture, bool enableBarriers)
{
    Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(itexture);
    m_stateTracker.SetEnableUavBarriersForTexture(texture, enableBarriers);
}

void CommandList::SetEnableUavBarriersForBuffer(IBuffer* ibuffer, bool enableBarriers)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);
    m_stateTracker.SetEnableUavBarriersForBuffer(buffer, enableBarriers);
}

void CommandList::BeginTrackingTextureState(ITexture* itexture, TextureSubresourceSet subresources, ResourceStates stateBits)
{
    Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(itexture);
    m_stateTracker.BeginTrackingTextureState(texture, subresources, stateBits);
}

void CommandList::BeginTrackingBufferState(IBuffer* ibuffer, ResourceStates stateBits)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);
    m_stateTracker.BeginTrackingBufferState(buffer, stateBits);
}

void CommandList::SetTextureState(ITexture* itexture, TextureSubresourceSet subresources, ResourceStates stateBits)
{
    Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(itexture);
    m_stateTracker.EndTrackingTextureState(texture, subresources, stateBits, false);
}

void CommandList::SetBufferState(IBuffer* ibuffer, ResourceStates stateBits)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);
    m_stateTracker.EndTrackingBufferState(buffer, stateBits, false);
}

void CommandList::SetAccelStructState(rt::IAccelStruct* ias, ResourceStates stateBits)
{
    AccelStruct* as = core::MemoryUtilities::CheckedCast<AccelStruct*>(ias);
    if (as->dataBuffer)
    {
        m_stateTracker.EndTrackingBufferState(as->dataBuffer, stateBits, false);
    }
}

void CommandList::SetPermanentTextureState(ITexture* itexture, ResourceStates stateBits)
{
    Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(itexture);
    m_stateTracker.EndTrackingTextureState(texture, AllSubresources, stateBits, true);
}

void CommandList::SetPermanentBufferState(IBuffer* ibuffer, ResourceStates stateBits)
{
    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);
    m_stateTracker.EndTrackingBufferState(buffer, stateBits, true);
}

void CommandList::CommitBarriers()
{
    const auto& textureBarriers = m_stateTracker.GetTextureBarriers();
    const auto& bufferBarriers = m_stateTracker.GetBufferBarriers();
    const size_t barrierCount = textureBarriers.size() + bufferBarriers.size();
    if (barrierCount == 0)
    {
        return;
    }

    m_d3dBarriers.clear();
    m_d3dBarriers.reserve(barrierCount);

    for (const auto& barrier : textureBarriers)
    {
        const Texture* texture = static_cast<const Texture*>(barrier.texture);

        D3D12_RESOURCE_BARRIER d3dbarrier {};
        const D3D12_RESOURCE_STATES stateBefore = D3D12Utility::ConvertResourceStates(barrier.stateBefore);
        const D3D12_RESOURCE_STATES stateAfter = D3D12Utility::ConvertResourceStates(barrier.stateAfter);
        if (stateBefore != stateAfter)
        {
            d3dbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            d3dbarrier.Transition.StateBefore = stateBefore;
            d3dbarrier.Transition.StateAfter = stateAfter;
            d3dbarrier.Transition.pResource = texture->resource;
            if (barrier.entireTexture)
            {
                d3dbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                m_d3dBarriers.push_back(d3dbarrier);
            }
            else
            {
                for (uint8_t plane = 0; plane < texture->planeCount; plane++)
                {
                    d3dbarrier.Transition.Subresource = D3D12Utility::CalcSubresource(barrier.mipLevel, barrier.arraySlice, plane, texture->desc.mipLevels, texture->desc.arraySize);
                    m_d3dBarriers.push_back(d3dbarrier);
                }
            }
        }
        else if (stateAfter & D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
        {
            d3dbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            d3dbarrier.UAV.pResource = texture->resource;
            m_d3dBarriers.push_back(d3dbarrier);
        }
    }

    for (const auto& barrier : bufferBarriers)
    {
        const Buffer* buffer = static_cast<const Buffer*>(barrier.buffer);

        D3D12_RESOURCE_BARRIER d3dbarrier {};
        const D3D12_RESOURCE_STATES stateBefore = D3D12Utility::ConvertResourceStates(barrier.stateBefore);
        const D3D12_RESOURCE_STATES stateAfter = D3D12Utility::ConvertResourceStates(barrier.stateAfter);
        if (stateBefore != stateAfter &&
            (stateBefore & D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE) == 0 &&
            (stateAfter & D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE) == 0)
        {
            d3dbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            d3dbarrier.Transition.StateBefore = stateBefore;
            d3dbarrier.Transition.StateAfter = stateAfter;
            d3dbarrier.Transition.pResource = buffer->resource;
            d3dbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            m_d3dBarriers.push_back(d3dbarrier);
        }
        else if (
            (barrier.stateBefore == ResourceStates::AccelStructWrite && barrier.stateAfter.Test(ResourceStates::AccelStructRead) && barrier.stateAfter.Test(ResourceStates::AccelStructBuildBlas)) ||
            (barrier.stateAfter == ResourceStates::AccelStructWrite && barrier.stateBefore.Test(ResourceStates::AccelStructRead) && barrier.stateBefore.Test(ResourceStates::AccelStructBuildBlas)) ||
            (barrier.stateBefore == ResourceStates::OpacityMicromapWrite && barrier.stateAfter.Test(ResourceStates::AccelStructBuildInput)) ||
            (stateAfter & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0)
        {
            d3dbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            d3dbarrier.UAV.pResource = buffer->resource;
            m_d3dBarriers.push_back(d3dbarrier);
        }
    }

    if (m_d3dBarriers.size() > 0)
    {
        m_activeCommandList->commandList->ResourceBarrier(uint32_t(m_d3dBarriers.size()), m_d3dBarriers.data());
    }

    m_stateTracker.ClearBarriers();
}

bool CommandList::AllocateUploadBuffer(size_t size, void** pCpuAddress, D3D12_GPU_VIRTUAL_ADDRESS* pGpuAddress)
{
    return m_uploadManager.SuballocateBuffer(size, nullptr, nullptr, nullptr, pCpuAddress, pGpuAddress, m_recordingVersion, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
}

bool CommandList::AllocateDxrScratchBuffer(size_t size, void** pCpuAddress, D3D12_GPU_VIRTUAL_ADDRESS* pGpuAddress)
{
    return m_dxrScratchManager.SuballocateBuffer(size, m_activeCommandList->commandList, nullptr, nullptr, pCpuAddress, pGpuAddress, m_recordingVersion, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
}

bool CommandList::CommitDescriptorHeaps()
{
    ID3D12DescriptorHeap* heapSRVetc = m_resources.shaderResourceViewHeap.GetShaderVisibleHeap();
    ID3D12DescriptorHeap* heapSamplers = m_resources.samplerHeap.GetShaderVisibleHeap();

    if (heapSRVetc != m_currentHeapSrvEtc || heapSamplers != m_currentHeapSamplers)
    {
        ID3D12DescriptorHeap* heaps[2] = { heapSRVetc, heapSamplers };
        m_activeCommandList->commandList->SetDescriptorHeaps(2, heaps);

        m_currentHeapSrvEtc = heapSRVetc;
        m_currentHeapSamplers = heapSamplers;

        m_instance->referencedNativeResources.push_back(heapSRVetc);
        m_instance->referencedNativeResources.push_back(heapSamplers);

        return true;
    }

    return false;
}

D3D12_GPU_VIRTUAL_ADDRESS CommandList::GetBufferGpuVA(IBuffer* ibuffer)
{
    if (!ibuffer)
    {
        return 0;
    }

    Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(ibuffer);

    if (buffer->desc.volatileBuffer)
    {
        auto buffer_adress_itr = m_volatileConstantBufferAddresses.find(buffer);
        if (buffer_adress_itr != m_volatileConstantBufferAddresses.end())
        {
            return buffer_adress_itr->second;
        }
    }

    return buffer->gpuVirtualAddress;
}

void CommandList::UpdateGraphicsVolatileBuffers()
{
    if (!m_anyVolatileBufferWrites)
    {
        return;
    }

    for (VolatileConstantBufferBinding& parameter : m_currentGraphicsVolatileCbs)
    {
        D3D12_GPU_VIRTUAL_ADDRESS currentGpuVA = 0;
        auto buffer_adress_itr = m_volatileConstantBufferAddresses.find(parameter.buffer);
        if (buffer_adress_itr != m_volatileConstantBufferAddresses.end())
        {
            currentGpuVA = buffer_adress_itr->second;
        }

        if (currentGpuVA != parameter.address)
        {
            m_activeCommandList->commandList->SetGraphicsRootConstantBufferView(parameter.bindingPoint, currentGpuVA);

            parameter.address = currentGpuVA;
        }
    }

    m_anyVolatileBufferWrites = false;
}

void CommandList::UpdateComputeVolatileBuffers()
{
    if (!m_anyVolatileBufferWrites)
    {
        return;
    }

    for (VolatileConstantBufferBinding& parameter : m_currentComputeVolatileCbs)
    {
        D3D12_GPU_VIRTUAL_ADDRESS currentGpuVA = 0;
        auto buffer_adress_itr = m_volatileConstantBufferAddresses.find(parameter.buffer);
        if (buffer_adress_itr != m_volatileConstantBufferAddresses.end())
        {
            currentGpuVA = buffer_adress_itr->second;
        }

        if (currentGpuVA != parameter.address)
        {
            m_activeCommandList->commandList->SetComputeRootConstantBufferView(parameter.bindingPoint, currentGpuVA);

            parameter.address = currentGpuVA;
        }
    }

    m_anyVolatileBufferWrites = false;
}

void CommandList::SetComputeBindings(const BindingSetVector& bindings, uint32_t bindingUpdateMask, IBuffer* indirectParams, bool updateIndirectParams, const RootSignature* rootSignature)
{
    if (bindingUpdateMask)
    {
        core::TStaticVector<VolatileConstantBufferBinding, MAX_VOLATILE_CONSTANT_BUFFERS> newVolatileCBs;

        for (uint32_t bindingSetIndex = 0; bindingSetIndex < uint32_t(bindings.size()); bindingSetIndex++)
        {
            IBindingSet* _bindingSet = bindings[bindingSetIndex];

            if (!_bindingSet)
            {
                continue;
            }

            const bool updateThisSet = (bindingUpdateMask & (1 << bindingSetIndex)) != 0;

            const core::TPair<BindingLayoutPtr, RootParameterIndex>& layoutAndOffset = rootSignature->pipelineLayouts[bindingSetIndex];
            RootParameterIndex rootParameterOffset = layoutAndOffset.second;

            if (_bindingSet->GetDesc())
            {
                SLUG_ASSERT(layoutAndOffset.first == _bindingSet->GetLayout());

                BindingSet* bindingSet = core::MemoryUtilities::CheckedCast<BindingSet*>(_bindingSet);

                for (size_t volatileCbIndex = 0; volatileCbIndex < bindingSet->rootParametersVolatileCb.size(); volatileCbIndex++)
                {
                    const auto& parameter = bindingSet->rootParametersVolatileCb[volatileCbIndex];
                    RootParameterIndex rootParameterIndex = rootParameterOffset + parameter.first;

                    if (parameter.second)
                    {
                        Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(parameter.second);

                        if (buffer->desc.volatileBuffer)
                        {
                            D3D12_GPU_VIRTUAL_ADDRESS volatileData = 0;
                            auto buffer_adress_itr = m_volatileConstantBufferAddresses.find(buffer);
                            if (buffer_adress_itr != m_volatileConstantBufferAddresses.end())
                            {
                                volatileData = buffer_adress_itr->second;
                            }

                            if (!volatileData)
                            {
                                continue;
                            }

                            if (updateThisSet || volatileData != m_currentComputeVolatileCbs[newVolatileCBs.size()].address)
                            {
                                m_activeCommandList->commandList->SetComputeRootConstantBufferView(rootParameterIndex, volatileData);
                            }

                            newVolatileCBs.push_back(VolatileConstantBufferBinding { rootParameterIndex, buffer, volatileData });
                        }
                        else if (updateThisSet)
                        {
                            SLUG_ASSERT(buffer->gpuVirtualAddress != 0);

                            m_activeCommandList->commandList->SetComputeRootConstantBufferView(rootParameterIndex, buffer->gpuVirtualAddress);
                        }
                    }
                    else if (updateThisSet)
                    {
                        m_activeCommandList->commandList->SetComputeRootConstantBufferView(rootParameterIndex, 0);
                    }
                }

                if (updateThisSet)
                {
                    if (bindingSet->descriptorTableValidSamplers)
                    {
                        m_activeCommandList->commandList->SetComputeRootDescriptorTable(
                            rootParameterOffset + bindingSet->rootParameterIndexSamplers,
                            m_resources.samplerHeap.GetGpuHandle(bindingSet->descriptorTableSamplers));
                    }

                    if (bindingSet->descriptorTableValidSrvEtc)
                    {
                        m_activeCommandList->commandList->SetComputeRootDescriptorTable(
                            rootParameterOffset + bindingSet->rootParameterIndexSrvEtc,
                            m_resources.shaderResourceViewHeap.GetGpuHandle(bindingSet->descriptorTableSrvEtc));
                    }

                    if (bindingSet->desc.trackLiveness)
                    {
                        m_instance->referencedResources.push_back(bindingSet);
                    }
                }

                if (m_enableAutomaticBarriers && (updateThisSet || bindingSet->hasUavBindings))
                {
                    SetResourceStatesForBindingSet(bindingSet);
                }
            }
            else
            {
                DescriptorTable* descriptorTable = core::MemoryUtilities::CheckedCast<DescriptorTable*>(_bindingSet);

                m_activeCommandList->commandList->SetComputeRootDescriptorTable(rootParameterOffset, m_resources.shaderResourceViewHeap.GetGpuHandle(descriptorTable->firstDescriptor));
            }
        }

        m_currentComputeVolatileCbs = newVolatileCBs;
    }

    if (indirectParams && updateIndirectParams)
    {
        if (m_enableAutomaticBarriers)
        {
            RequireBufferState(indirectParams, ResourceStates::IndirectArgument);
        }
        m_instance->referencedResources.push_back(indirectParams);
    }

    uint32_t bindingMask = (1 << uint32_t(bindings.size())) - 1;
    if ((bindingUpdateMask & bindingMask) == bindingMask)
    {
        m_anyVolatileBufferWrites = false;
    }
}

void CommandList::SetGraphicsBindings(const BindingSetVector& bindings, uint32_t bindingUpdateMask, IBuffer* indirectParams, bool updateIndirectParams, const RootSignature* rootSignature)
{
    if (bindingUpdateMask)
    {
        core::TStaticVector<VolatileConstantBufferBinding, MAX_VOLATILE_CONSTANT_BUFFERS> newVolatileCBs;

        for (uint32_t bindingSetIndex = 0; bindingSetIndex < uint32_t(bindings.size()); bindingSetIndex++)
        {
            IBindingSet* _bindingSet = bindings[bindingSetIndex];

            if (!_bindingSet)
            {
                continue;
            }

            const bool updateThisSet = (bindingUpdateMask & (1 << bindingSetIndex)) != 0;

            const core::TPair<BindingLayoutPtr, RootParameterIndex>& layoutAndOffset = rootSignature->pipelineLayouts[bindingSetIndex];
            RootParameterIndex rootParameterOffset = layoutAndOffset.second;

            if (_bindingSet->GetDesc())
            {
                SLUG_ASSERT(layoutAndOffset.first == _bindingSet->GetLayout());

                BindingSet* bindingSet = core::MemoryUtilities::CheckedCast<BindingSet*>(_bindingSet);

                for (size_t volatileCbIndex = 0; volatileCbIndex < bindingSet->rootParametersVolatileCb.size(); volatileCbIndex++)
                {
                    const auto& parameter = bindingSet->rootParametersVolatileCb[volatileCbIndex];
                    RootParameterIndex rootParameterIndex = rootParameterOffset + parameter.first;

                    if (parameter.second)
                    {
                        Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(parameter.second);

                        if (buffer->desc.volatileBuffer)
                        {
                            D3D12_GPU_VIRTUAL_ADDRESS volatileData = 0;
                            auto buffer_adress_itr = m_volatileConstantBufferAddresses.find(buffer);
                            if (buffer_adress_itr != m_volatileConstantBufferAddresses.end())
                            {
                                volatileData = buffer_adress_itr->second;
                            }

                            if (!volatileData)
                            {
                                core::String str = core::StringUtility::Sprintfs("Attempted use of a volatile constant buffer %s before it was written into", buffer->desc.debugName.c_str());
                                SLUG_LOG(RHILogCategory, core::LogType::Error, str.c_str());
                                continue;
                            }

                            if (updateThisSet || volatileData != m_currentGraphicsVolatileCbs[newVolatileCBs.size()].address)
                            {
                                m_activeCommandList->commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, volatileData);
                            }

                            newVolatileCBs.push_back(VolatileConstantBufferBinding { rootParameterIndex, buffer, volatileData });
                        }
                        else if (updateThisSet)
                        {
                            SLUG_ASSERT(buffer->gpuVirtualAddress != 0);
                            m_activeCommandList->commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, buffer->gpuVirtualAddress);
                        }
                    }
                    else if (updateThisSet)
                    {
                        m_activeCommandList->commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, 0);
                    }
                }

                if (updateThisSet)
                {
                    if (bindingSet->descriptorTableValidSamplers)
                    {
                        m_activeCommandList->commandList->SetGraphicsRootDescriptorTable(
                            rootParameterOffset + bindingSet->rootParameterIndexSamplers,
                            m_resources.samplerHeap.GetGpuHandle(bindingSet->descriptorTableSamplers));
                    }

                    if (bindingSet->descriptorTableValidSrvEtc)
                    {
                        m_activeCommandList->commandList->SetGraphicsRootDescriptorTable(
                            rootParameterOffset + bindingSet->rootParameterIndexSrvEtc,
                            m_resources.shaderResourceViewHeap.GetGpuHandle(bindingSet->descriptorTableSrvEtc));
                    }

                    if (bindingSet->desc.trackLiveness)
                    {
                        m_instance->referencedResources.push_back(bindingSet);
                    }
                }

                if (m_enableAutomaticBarriers && (updateThisSet || bindingSet->hasUavBindings))
                {
                    SetResourceStatesForBindingSet(bindingSet);
                }
            } else if (updateThisSet)
            {
                DescriptorTable* descriptorTable = core::MemoryUtilities::CheckedCast<DescriptorTable*>(_bindingSet);

                m_activeCommandList->commandList->SetGraphicsRootDescriptorTable(rootParameterOffset, m_resources.shaderResourceViewHeap.GetGpuHandle(descriptorTable->firstDescriptor));
            }
        }

        m_currentGraphicsVolatileCbs = newVolatileCBs;
    }

    if (indirectParams && updateIndirectParams)
    {
        if (m_enableAutomaticBarriers)
        {
            RequireBufferState(indirectParams, ResourceStates::IndirectArgument);
        }
        m_instance->referencedResources.push_back(indirectParams);
    }

    uint32_t bindingMask = (1 << uint32_t(bindings.size())) - 1;
    if ((bindingUpdateMask & bindingMask) == bindingMask)
    {
        m_anyVolatileBufferWrites = false;
    }
}

ShaderTableState* CommandList::GetShaderTableStateTracking(rt::IShaderTable* ishaderTable)
{
    auto it = m_shaderTableStates.find(ishaderTable);

    if (it != m_shaderTableStates.end())
    {
        return it->second.get();
    }

    core::TUniquePtr<ShaderTableState> trackingRef = core::MakeUnique<ShaderTableState>();

    ShaderTableState* tracking = trackingRef.get();
    m_shaderTableStates.insert(core::MakePair(ishaderTable, std::move(trackingRef)));

    return tracking;
}

void CommandList::ClearStateCache()
{
    m_anyVolatileBufferWrites = false;
    m_currentGraphicsStateValid = false;
    m_currentComputeStateValid = false;
    m_currentMeshletStateValid = false;
    m_currentRayTracingStateValid = false;
    m_currentHeapSrvEtc = nullptr;
    m_currentHeapSamplers = nullptr;
    m_currentGraphicsVolatileCbs.resize(0);
    m_currentComputeVolatileCbs.resize(0);
    m_currentSinglePassStereoState = SinglePassStereoState();
}

void CommandList::BindGraphicsPipeline(GraphicsPipeline* pso, bool updateRootSignature) const
{
    const auto& pipelineDesc = pso->desc;

    if (updateRootSignature)
    {
        m_activeCommandList->commandList->SetGraphicsRootSignature(pso->rootSignature->handle);
    }

    m_activeCommandList->commandList->SetPipelineState(pso->pipelineState);

    m_activeCommandList->commandList->IASetPrimitiveTopology(D3D12Utility::ConvertPrimitiveType(pipelineDesc.primitiveType, pipelineDesc.patchControlPoints));

    if (pipelineDesc.renderState.depthStencilState.stencilEnable)
    {
        m_activeCommandList->commandList->OMSetStencilRef(pipelineDesc.renderState.depthStencilState.stencilRefValue);
    }
}
void CommandList::BindMeshletPipeline(MeshletPipeline* pso, bool updateRootSignature) const
{
    const auto& state = pso->desc;

    ID3D12GraphicsCommandList* commandList = m_activeCommandList->commandList;

    if (updateRootSignature)
    {
        commandList->SetGraphicsRootSignature(pso->rootSignature->handle);
    }

    commandList->SetPipelineState(pso->pipelineState);

    commandList->IASetPrimitiveTopology(D3D12Utility::ConvertPrimitiveType(state.primitiveType, 0));

    if (pso->viewportState.numViewports)
    {
        commandList->RSSetViewports(pso->viewportState.numViewports, pso->viewportState.viewports);
    }

    if (pso->viewportState.numScissorRects)
    {
        commandList->RSSetScissorRects(pso->viewportState.numViewports, pso->viewportState.scissorRects);
    }

    if (state.renderState.depthStencilState.stencilEnable)
    {
        commandList->OMSetStencilRef(state.renderState.depthStencilState.stencilRefValue);
    }
}

void CommandList::BindFramebuffer(FrameBuffer* fb)
{
    if (m_enableAutomaticBarriers)
    {
        SetResourceStatesForFrameBuffer(fb);
    }

    core::TStaticVector<D3D12_CPU_DESCRIPTOR_HANDLE, 16> RTVs;
    for (uint32_t rtIndex = 0; rtIndex < fb->rtvs.size(); rtIndex++)
    {
        RTVs.push_back(m_resources.renderTargetViewHeap.GetCpuHandle(fb->rtvs[rtIndex]));
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DSV = {};
    if (fb->desc.depthAttachment.Valid())
    {
        DSV = m_resources.depthStencilViewHeap.GetCpuHandle(fb->dsv);
    }

    m_activeCommandList->commandList->OMSetRenderTargets(UINT(RTVs.size()), RTVs.data(), false, fb->desc.depthAttachment.Valid() ? &DSV : nullptr);
}

void CommandList::UnbindShadingRateState()
{
    if (m_currentGraphicsStateValid && m_currentGraphicsState.shadingRateState.enabled)
    {
        m_activeCommandList->commandList6->RSSetShadingRateImage(nullptr);
        m_activeCommandList->commandList6->RSSetShadingRate(D3D12_SHADING_RATE_1X1, nullptr);
        m_currentGraphicsState.shadingRateState.enabled = false;
        m_currentGraphicsState.framebuffer = nullptr;
    }
}

core::TReferencePtr<InternalCommandList> CommandList::CreateInternalCommandList() const
{
    core::TReferencePtr<InternalCommandList> commandList = core::MakeReference<InternalCommandList>();

    D3D12_COMMAND_LIST_TYPE d3dCommandListType;
    switch (m_desc.queueType)
    {
    case CommandQueue::Graphics:
        d3dCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
        break;
    case CommandQueue::Compute:
        d3dCommandListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        break;
    case CommandQueue::Copy:
        d3dCommandListType = D3D12_COMMAND_LIST_TYPE_COPY;
        break;

    case CommandQueue::Count:
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Error, "Invalid Enum CommandQueue");
        return nullptr;
    }

    m_context.device->CreateCommandAllocator(d3dCommandListType, IID_PPV_ARGS(&commandList->allocator));
    m_context.device->CreateCommandList(0, d3dCommandListType, commandList->allocator, nullptr, IID_PPV_ARGS(&commandList->commandList));

    commandList->commandList->QueryInterface(IID_PPV_ARGS(&commandList->commandList4));
    commandList->commandList->QueryInterface(IID_PPV_ARGS(&commandList->commandList6));

    return commandList;
}

void CommandList::BuildTopLevelAccelStructInternal(AccelStruct* as, D3D12_GPU_VIRTUAL_ADDRESS instanceData, size_t numInstances, rt::AccelStructBuildFlagsBitset buildFlags)
{
    buildFlags.Reset(rt::AccelStructBuildFlags::AllowEmptyInstances);

    const bool performUpdate = (buildFlags.Test(rt::AccelStructBuildFlags::PerformUpdate));

    if (performUpdate)
    {
        SLUG_ASSERT(as->allowUpdate);
        SLUG_ASSERT(as->dxrInstances.size() == numInstances);
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS ASInputs;
    ASInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    ASInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    ASInputs.InstanceDescs = instanceData;
    ASInputs.NumDescs = UINT(numInstances);
    ASInputs.Flags = D3D12Utility::ConvertAcceslerationStructureBuildFlags(buildFlags);
    if (as->allowUpdate)
    {
        ASInputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
    }

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO ASPreBuildInfo = {};
    m_context.device5->GetRaytracingAccelerationStructurePrebuildInfo(&ASInputs, &ASPreBuildInfo);

    if (ASPreBuildInfo.ResultDataMaxSizeInBytes > as->dataBuffer->desc.byteSize)
    {
        core::String str = core::StringUtility::Sprintfs("TLAS %s build requires at least %d bytes in the data buffer, while the allocated buffer is only %d bytes",
                                                         as->desc.debugName.c_str(), ASPreBuildInfo.ResultDataMaxSizeInBytes, as->dataBuffer->desc.byteSize);
        SLUG_LOG(RHILogCategory, core::LogType::Error, str.c_str());
        return;
    }

    uint64_t scratchSize = performUpdate
        ? ASPreBuildInfo.UpdateScratchDataSizeInBytes
        : ASPreBuildInfo.ScratchDataSizeInBytes;

    D3D12_GPU_VIRTUAL_ADDRESS scratchGpuVA = 0;
    if (!m_dxrScratchManager.SuballocateBuffer(scratchSize, m_activeCommandList->commandList, nullptr, nullptr, nullptr,
                                                 &scratchGpuVA, m_recordingVersion, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT))
    {
        core::String str = core::StringUtility::Sprintfs("Couldn't suballocate a scratch buffer for TLAS %s The build requires %d bytes of scratch space.",
                                                         as->desc.debugName.c_str(), scratchSize);
        SLUG_LOG(RHILogCategory, core::LogType::Error, str.c_str());
        return;
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
    buildDesc.Inputs = ASInputs;
    buildDesc.ScratchAccelerationStructureData = scratchGpuVA;
    buildDesc.DestAccelerationStructureData = as->dataBuffer->gpuVirtualAddress;
    buildDesc.SourceAccelerationStructureData = performUpdate ? as->dataBuffer->gpuVirtualAddress : 0;
    m_activeCommandList->commandList4->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
}

}
}
}
