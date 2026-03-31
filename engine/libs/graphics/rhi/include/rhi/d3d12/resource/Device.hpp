#pragma once

#include "rhi/d3d12/interface/IDevice.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/CommandList.hpp"
#include "rhi/d3d12/resource/Context.hpp"

namespace slug 
{
namespace rhi
{
namespace d3d12
{

class Device final : public rhi::d3d12::IDevice
{
public:
    static rhi::DevicePtr Create(const DeviceDesc& desc);

public:

    explicit Device(const DeviceDesc& desc);
    ~Device() override;

    // Context
    Context& GetContext();

    // GetAPI
    Object GetNativeObject(ObjectType objectType) override;
    GraphicsApiType GetGraphicsAPI() override;
    bool GetNvapiIsInitialized() const;

    // Queue
    Queue* GetQueue(CommandQueue type);
    Object GetNativeQueue(ObjectType objectType, CommandQueue queue) override;

    // Feature
    bool QueryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) override;
    FormatSupportBitset QueryFormatSupport(FormatType format) override;

    // Heap
    HeapPtr CreateHeap(const HeapDesc& d) override;

    // Texture
    TexturePtr CreateTexture(const TextureDesc& d) override;
    MemoryRequirements GetTextureMemoryRequirements(ITexture* texture) override;
    bool BindTextureMemory(ITexture* texture, IHeap* heap, uint64_t offset) override;
    TexturePtr CreateHandleForNativeTexture(ObjectType objectType, Object texture, const TextureDesc& desc) override;
    StagingTexturePtr CreateStagingTexture(const TextureDesc& d, CpuAccessMode cpuAccess) override;
    void* MapStagingTexture(IStagingTexture* tex, const TextureSlice& slice, CpuAccessMode cpuAccess, size_t* outRowPitch) override;
    void UnmapStagingTexture(IStagingTexture* tex) override;

    // Sampler
    SamplerPtr CreateSampler(const SamplerDesc& d) override;

    // Buffer
    BufferPtr CreateBuffer(const BufferDesc& d) override;
    void* MapBuffer(IBuffer* b, CpuAccessMode mapFlags) override;
    void UnmapBuffer(IBuffer* b) override;
    MemoryRequirements GetBufferMemoryRequirements(IBuffer* buffer) override;
    bool BindBufferMemory(IBuffer* buffer, IHeap* heap, uint64_t offset) override;
    BufferPtr CreateHandleForNativeBuffer(ObjectType objectType, Object buffer, const BufferDesc& desc) override;

    // Shader
    ShaderPtr CreateShader(const ShaderDesc& desc, const void* binary, size_t binarysize_t) override;
    ShaderPtr CreateShaderSpecialization(IShader* baseShader, const ShaderSpecialization* constants, uint32_t numConstants) override;
    ShaderLibraryPtr CreateShaderLibrary(const void* binary, size_t binarysize_t) override;
    bool SetHlslExtensionsUAV(uint32_t slot);

    // InputLayout
    InputLayoutPtr CreateInputLayout(const VertexAttributeDesc* desc, uint32_t attributeCount, IShader* vertexShader) override;

    // EventQuery
    EventQueryPtr CreateEventQuery() override;
    void SetEventQuery(IEventQuery* query, CommandQueue queue) override;
    bool PollEventQuery(IEventQuery* query) override;
    void WaitEventQuery(IEventQuery* query) override;
    void ResetEventQuery(IEventQuery* query) override;

    // TimeQuery
    TimerQueryPtr CreateTimerQuery() override;
    bool PollTimerQuery(ITimerQuery* query) override;
    float GetTimerQueryTime(ITimerQuery* query) override;
    void ResetTimerQuery(ITimerQuery* query) override;

    // FrameBuffer
    FrameBufferPtr CreateFrameBuffer(const FrameBufferDesc& desc) override;

    // GraphicsPipeline
    GraphicsPipelinePtr CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, IFrameBuffer* fb) override;
    GraphicsPipelinePtr CreateGraphicsPipelineFromCache(const PipelineCache& pipelineCache, GraphicsPipelineDesc& outDesc) override;
    GraphicsPipelinePtr CreateHandleForNativeGraphicsPipeline(IRootSignature* rootSignature, ID3D12PipelineState* pipelineState, const GraphicsPipelineDesc& desc, const FrameBufferInfo& framebufferInfo) override;

    // ComputePipeline
    ComputePipelinePtr CreateComputePipeline(const ComputePipelineDesc& desc) override;
    ComputePipelinePtr CreateComputePipelineFromCache(const PipelineCache& pipelineCache, ComputePipelineDesc& outDesc);

    // MeshletPipeline
    MeshletPipelinePtr CreateMeshletPipeline(const MeshletPipelineDesc& desc, IFrameBuffer* fb) override;
    MeshletPipelinePtr CreateHandleForNativeMeshletPipeline(IRootSignature* rootSignature, ID3D12PipelineState* pipelineState, const MeshletPipelineDesc& desc, const FrameBufferInfo& framebufferInfo) override;

    // RaytracingPipeline
    rt::PipelinePtr CreateRayTracingPipeline(const rt::PipelineDesc& desc) override;

    // BindLayout/BindingSet
    BindingLayoutPtr CreateBindingLayout(const BindingLayoutDesc& desc) override;
    BindingLayoutPtr CreateBindlessLayout(const BindlessLayoutDesc& desc) override;
    BindingSetPtr CreateBindingSet(const BindingSetDesc& desc, IBindingLayout* layout) override;

    // DescriptorTable
    DescriptorTablePtr CreateDescriptorTable(IBindingLayout* layout) override;
    void ResizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t capacity, bool keepContents = true) override;
    bool WriteDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item) override;
    IDescriptorHeap* GetDescriptorHeap(DescriptorHeapType heapType) override;

    // RootSigunature
    RootSignaturePtr BuildRootSignature(const core::TStaticVector<BindingLayoutPtr, MAX_BINDING_LAYOUTS>& pipelineLayouts, bool allowInputLayout, bool isLocal, const D3D12_ROOT_PARAMETER1* pCustomParameters = nullptr, uint32_t numCustomParameters = 0) override;

    // Raytracing
    rt::OpacityMicromapPtr CreateOpacityMicromap(const rt::OpacityMicromapDesc& desc) override;
    rt::AccelStructPtr CreateAccelStruct(const rt::AccelStructDesc& desc) override;
    MemoryRequirements GetAccelStructMemoryRequirements(rt::IAccelStruct* as) override;
    bool BindAccelStructMemory(rt::IAccelStruct* as, IHeap* heap, uint64_t offset) override;
    bool GetAccelStructPreBuildInfo(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& outPreBuildInfo, const rt::AccelStructDesc& desc) const;

    // CommandList
    rhi::CommandListPtr CreateCommandList(const CommandListParameters& params = CommandListParameters()) override;
    uint64_t ExecuteCommandLists(rhi::ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue = CommandQueue::Graphics) override;
    void QueueWaitForCommandList(CommandQueue waitQueue, CommandQueue executionQueue, uint64_t instance) override;

    // Flush
    void WaitForIdle() override;
    void RunGarbageCollection() override;

private:
    core::TReferencePtr<RootSignature> GetRootSignature(const core::TStaticVector<BindingLayoutPtr, MAX_BINDING_LAYOUTS>& pipelineLayouts, bool allowInputLayout, core::TReferencePtr<RootSignature> pRS = nullptr);

    // GraphicsPipeline
    core::TReferencePtr<ID3D12PipelineState> CreatePipelineState(const GraphicsPipelineDesc& desc, RootSignature* pRS, const FrameBufferInfo& fbinfo) const;
    core::TReferencePtr<ID3D12PipelineState> CreatePipelineStateFromCache(const PipelineCache& pipelineCache, GraphicsPipelineDesc& outDesc, core::TReferencePtr<RootSignature>& outRootSignature, FrameBufferInfoEx& outFrameBufferInfo);
    core::TReferencePtr<ID3D12PipelineState> CreatePipelineState(const ComputePipelineDesc& desc, RootSignature* pRS) const;
    core::TReferencePtr<ID3D12PipelineState> CreatePipelineStateFromCache(const PipelineCache& pipelineCache, ComputePipelineDesc& outDesc, core::TReferencePtr<RootSignature>& outRootSignature);
    core::TReferencePtr<ID3D12PipelineState> CreatePipelineState(const MeshletPipelineDesc& desc, RootSignature* pRS, const FrameBufferInfo& fbinfo) const;
    void RegisterShaderCode(ShaderPtr& srcShader, const void* dstShaderBytecodePointer, size_t dstBytecodeLength) const;

private:
    Context m_context;
    DeviceResources m_resources;

    core::TArray<core::TUniquePtr<Queue>, (int32_t)CommandQueue::Count> m_queues;
    HANDLE m_fenceEvent;

    std::mutex m_mutex;

    core::TVector<ID3D12CommandList*> m_commandListsToExecute;
    bool m_nvapiIsInitialized = false;
    bool m_singlePassStereoSupported = false;
    bool m_fastGeometryShaderSupported = false;
    bool m_rayTracingSupported = false;
    bool m_traceRayInlineSupported = false;
    bool m_meshletsSupported = false;
    bool m_variableRateShadingSupported = false;
    bool m_opacityMicromapSupported = false;
    bool m_shaderExecutionReorderingSupported = false;

    D3D12_FEATURE_DATA_D3D12_OPTIONS  m_options = {};
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 m_options5 = {};
    D3D12_FEATURE_DATA_D3D12_OPTIONS6 m_options6 = {};
    D3D12_FEATURE_DATA_D3D12_OPTIONS7 m_options7 = {};
};

}
}
}
