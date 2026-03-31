#pragma once

#include "core/container/Deque.hpp"
#include "core/container/List.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/interface/ICommandList.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/StagingTexture.hpp"
#include "rhi/d3d12/resource/Buffer.hpp"
#include "rhi/d3d12/resource/TimerQuery.hpp"
#include "rhi/d3d12/resource/ShaderTable.hpp"
#include "rhi/d3d12/resource/GraphicsPipeline.hpp"
#include "rhi/d3d12/resource/MeshletPipeline.hpp"
#include "rhi/d3d12/resource/FrameBuffer.hpp"
#include "rhi/d3d12/resource/AccelStruct.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

class CommandListInstance : public core::ReferenceObject
{
public:
    uint64_t submittedInstance = 0;
    CommandQueue commandQueue = CommandQueue::Graphics;
    core::TReferencePtr<ID3D12Fence> fence;
    core::TReferencePtr<ID3D12CommandAllocator> commandAllocator;
    core::TReferencePtr<ID3D12CommandList> commandList;
    core::TVector<core::TReferencePtr<IResource>> referencedResources;
    core::TVector<core::TReferencePtr<IUnknown>> referencedNativeResources;
    core::TVector<core::TReferencePtr<StagingTexture>> referencedStagingTextures;
    core::TVector<core::TReferencePtr<Buffer>> referencedStagingBuffers;
    core::TVector<core::TReferencePtr<TimerQuery>> referencedTimerQueries;
};

class Queue
{
public:
    explicit Queue(const Context& context, ID3D12CommandQueue* queue);
    uint64_t UpdateLastCompletedInstance();
public:
    core::TReferencePtr<ID3D12CommandQueue> queue;
    core::TReferencePtr<ID3D12Fence> fence;
    uint64_t lastSubmittedInstance = 0;
    uint64_t lastCompletedInstance = 0;
    core::TAtomic<uint64_t> recordingInstance = 1;
    core::TDeque<core::TReferencePtr<class CommandListInstance>> commandListsInFlight;
private:
    const Context& m_context;
};

class BufferChunk : public core::ReferenceObject
{
public:
    ~BufferChunk();
public:
    static const uint64_t SIZE_ALIGNMENT = 4096;

    core::TReferencePtr<ID3D12Resource> buffer;
    uint64_t version = 0;
    uint64_t bufferSize = 0;
    uint64_t writePointer = 0;
    void* cpuVa = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpuVa = 0;
    uint32_t identifier = 0;
};

class UploadManager
{
public:
    UploadManager(const Context& context, class Queue* pQueue, size_t defaultChunkSize, uint64_t memoryLimit, bool isScratchBuffer);
    bool SuballocateBuffer(uint64_t size, ID3D12GraphicsCommandList* pCommandList, ID3D12Resource** pBuffer, size_t* pOffset, void** pCpuVa, D3D12_GPU_VIRTUAL_ADDRESS* pGpuVa, uint64_t currentVersion, uint32_t alignment = 256);
    void SubmitChunks(uint64_t currentVersion, uint64_t submittedVersion);

private:
    SLUG_NODISCARD core::TReferencePtr<BufferChunk> CreateChunk(size_t size) const;

private:
    const Context& m_context;
    Queue* m_queue;
    size_t m_defaultChunkSize = 0;
    uint64_t m_memoryLimit = 0;
    uint64_t m_allocatedMemory = 0;
    bool m_isScratchBuffer = false;

    core::TList<core::TReferencePtr<BufferChunk>> m_chunkPool;
    core::TReferencePtr<BufferChunk> m_currentChunk;
};

class InternalCommandList : public core::ReferenceObject
{
public:
    core::TReferencePtr<ID3D12CommandAllocator> allocator;
    core::TReferencePtr<ID3D12GraphicsCommandList> commandList;
    core::TReferencePtr<ID3D12GraphicsCommandList4> commandList4;
    core::TReferencePtr<ID3D12GraphicsCommandList6> commandList6;
    uint64_t lastSubmittedInstance = 0;
};

class CommandList final : public rhi::d3d12::ICommandList
{
public:
    CommandList(class Device* device, const Context& context, DeviceResources& resources, const CommandListParameters& params);

    core::TReferencePtr<CommandListInstance> Executed(Queue* pQueue);

    void RequireTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates state);
    void RequireBufferState(IBuffer* buffer, ResourceStates state);

    Object GetNativeObject(ObjectType objectType) override;
    const CommandListParameters& GetDesc() override;

    CommandListState GetCurrentState() override;

    ResourceStatesBitset GetTextureSubresourceState(ITexture* texture, ArraySlice arraySlice, MipLevel mipLevel) override;
    ResourceStatesBitset GetBufferState(IBuffer* buffer) override;

    ID3D12CommandList* GetD3D12CommandList() const;
    rhi::IDevice* GetDevice() override;

    void Open() override;
    void Close() override;
    void ClearState() override;

    void ClearTextureFloat(ITexture* t, TextureSubresourceSet subresources, const Color& clearColor) override;
    void ClearDepthStencilTexture(ITexture* t, TextureSubresourceSet subresources, bool clearDepth, float depth, bool clearStencil, uint8_t stencil) override;
    void ClearTextureUint(ITexture* t, TextureSubresourceSet subresources, uint32_t clearColor) override;

    void CopyTexture(ITexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice) override;
    void CopyTexture(IStagingTexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice) override;
    void CopyTexture(ITexture* dest, const TextureSlice& destSlice, IStagingTexture* src, const TextureSlice& srcSlice) override;
    void WriteTexture(ITexture* dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch) override;
    void ResolveTexture(ITexture* dest, const TextureSubresourceSet& dstSubresources, ITexture* src, const TextureSubresourceSet& srcSubresources) override;

    void WriteBuffer(IBuffer* b, const void* data, size_t data_size, uint64_t destOffsetBytes = 0) override;
    void ClearBufferUInt(IBuffer* b, uint32_t clearValue) override;
    void CopyBuffer(IBuffer* dest, uint64_t destOffsetBytes, IBuffer* src, uint64_t srcOffsetBytes, uint64_t dataSizeBytes) override;

    void SetPushConstants(const void* data, size_t byteSize) override;

    void SetGraphicsState(const GraphicsState& state) override;
    void Draw(const DrawArguments& args) override;
    void DrawIndexed(const DrawArguments& args) override;
    void DrawIndirect(uint32_t offsetBytes, uint32_t drawCount) override;
    void DrawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount) override;

    void SetComputeState(const ComputeState& state) override;
    void Dispatch(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) override;
    void DispatchIndirect(uint32_t offsetBytes) override;

    void SetMeshletState(const MeshletState& state) override;
    void DispatchMesh(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) override;

    void SetRayTracingState(const rt::State& state) override;
    void DispatchRays(const rt::DispatchRaysArguments& args) override;

    void BuildOpacityMicromap(rt::IOpacityMicromap* omm, const rt::OpacityMicromapDesc& desc) override;
    void BuildBottomLevelAccelStruct(rt::IAccelStruct* as, const rt::GeometryDesc* pGeometries, size_t numGeometries, rt::AccelStructBuildFlagsBitset buildFlags) override;
    void CompactBottomLevelAccelStructs() override;
    void BuildTopLevelAccelStruct(rt::IAccelStruct* as, const rt::InstanceDesc* pInstances, size_t numInstances, rt::AccelStructBuildFlagsBitset buildFlags) override;
    void BuildTopLevelAccelStructFromBuffer(rt::IAccelStruct* as, rhi::IBuffer* instanceBuffer, uint64_t instanceBufferOffset, size_t numInstances, rt::AccelStructBuildFlagsBitset buildFlags = rt::AccelStructBuildFlags::None) override;

    void BeginTimerQuery(ITimerQuery* query) override;
    void EndTimerQuery(ITimerQuery* query) override;

    void BeginMarker(core::StringView name) override;
    void EndMarker() override;

    void SetEnableAutomaticBarriers(bool enable) override;
    void SetResourceStatesForBindingSet(IBindingSet* bindingSet) override;

    void SetEnableUavBarriersForTexture(ITexture* texture, bool enableBarriers) override;
    void SetEnableUavBarriersForBuffer(IBuffer* buffer, bool enableBarriers) override;

    void BeginTrackingTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) override;
    void BeginTrackingBufferState(IBuffer* buffer, ResourceStates stateBits) override;

    void SetTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) override;
    void SetBufferState(IBuffer* buffer, ResourceStates stateBits) override;
    void SetAccelStructState(rt::IAccelStruct* as, ResourceStates stateBits) override;

    void SetPermanentTextureState(ITexture* texture, ResourceStates stateBits) override;
    void SetPermanentBufferState(IBuffer* buffer, ResourceStates stateBits) override;

    void CommitBarriers() override;

    bool AllocateUploadBuffer(size_t size, void** pCpuAddress, D3D12_GPU_VIRTUAL_ADDRESS* pGpuAddress) override;
    bool AllocateDxrScratchBuffer(size_t size, void** pCpuAddress, D3D12_GPU_VIRTUAL_ADDRESS* pGpuAddress);
    bool CommitDescriptorHeaps() override;
    D3D12_GPU_VIRTUAL_ADDRESS GetBufferGpuVA(IBuffer* buffer) override;

    void UpdateGraphicsVolatileBuffers() override;
    void UpdateComputeVolatileBuffers() override;
    void SetComputeBindings(const BindingSetVector& bindings, uint32_t bindingUpdateMask, IBuffer* indirectParams, bool updateIndirectParams, const RootSignature* rootSignature);
    void SetGraphicsBindings(const BindingSetVector& bindings, uint32_t bindingUpdateMask, IBuffer* indirectParams, bool updateIndirectParams, const RootSignature* rootSignature);

private:
    ShaderTableState* GetShaderTableStateTracking(rt::IShaderTable* shaderTable);
    void ClearStateCache();
    void BindGraphicsPipeline(GraphicsPipeline* pso, bool updateRootSignature) const;
    void BindMeshletPipeline(MeshletPipeline* pso, bool updateRootSignature) const;
    void BindFramebuffer(FrameBuffer* fb);
    void UnbindShadingRateState();
    core::TReferencePtr<InternalCommandList> CreateInternalCommandList() const;
    void BuildTopLevelAccelStructInternal(AccelStruct* as, D3D12_GPU_VIRTUAL_ADDRESS instanceData, size_t numInstances, rt::AccelStructBuildFlagsBitset buildFlags);

private:
    struct VolatileConstantBufferBinding
    {
        uint32_t bindingPoint;
        Buffer* buffer;
        D3D12_GPU_VIRTUAL_ADDRESS address;
    };

    const Context& m_context;
    DeviceResources& m_resources;
    IDevice* m_device;
    Queue* m_queue;
    UploadManager m_uploadManager;
    UploadManager m_dxrScratchManager;
    CommandListResourceStateTracker m_stateTracker;
    bool m_enableAutomaticBarriers = true;

    CommandListParameters m_desc;

    core::TReferencePtr<InternalCommandList> m_activeCommandList;
    core::TList<core::TReferencePtr<InternalCommandList>> m_commandListPool;
    core::TReferencePtr<CommandListInstance> m_instance;
    uint64_t m_recordingVersion = 0;

    GraphicsState m_currentGraphicsState;
    ComputeState m_currentComputeState;
    MeshletState m_currentMeshletState;
    rt::State m_currentRayTracingState;
    bool m_currentGraphicsStateValid = false;
    bool m_currentComputeStateValid = false;
    bool m_currentMeshletStateValid = false;
    bool m_currentRayTracingStateValid = false;


    ID3D12DescriptorHeap* m_currentHeapSrvEtc = nullptr;
    ID3D12DescriptorHeap* m_currentHeapSamplers = nullptr;
    ID3D12Resource* m_currentUploadBuffer = nullptr;
    SinglePassStereoState m_currentSinglePassStereoState;

    core::TUnorderedMap<IBuffer*, D3D12_GPU_VIRTUAL_ADDRESS> m_volatileConstantBufferAddresses;
    bool m_anyVolatileBufferWrites = false;

    core::TVector<D3D12_RESOURCE_BARRIER> m_d3dBarriers;
    core::TStaticVector<VolatileConstantBufferBinding, MAX_VOLATILE_CONSTANT_BUFFERS> m_currentGraphicsVolatileCbs;
    core::TStaticVector<VolatileConstantBufferBinding, MAX_VOLATILE_CONSTANT_BUFFERS> m_currentComputeVolatileCbs;

    core::TUnorderedMap<rt::IShaderTable*, core::TUniquePtr<ShaderTableState>> m_shaderTableStates;
};
}
}
}
