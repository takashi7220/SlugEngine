#pragma once

#include "rhi/interface/ICommandList.hpp"

namespace slug::rhi
{
class IDevice : public IResource 
{
public:
    // Heap
    virtual HeapPtr CreateHeap(const HeapDesc& heapDesc) = 0;

    // Texture
    virtual TexturePtr CreateTexture(const TextureDesc& textureDesc) = 0;
    virtual MemoryRequirements GetTextureMemoryRequirements(ITexture* texture) = 0;
    virtual bool BindTextureMemory(ITexture* texture, IHeap* heap, uint64_t offset) = 0;
    virtual TexturePtr CreateHandleForNativeTexture(ObjectType objectType, Object texture, const TextureDesc& textuerDesc) = 0;
    virtual StagingTexturePtr CreateStagingTexture(const TextureDesc& textureDesc, CpuAccessMode cpuAccess) = 0;
    virtual void* MapStagingTexture(IStagingTexture* texture, const TextureSlice& slice, CpuAccessMode cpuAccess, size_t* outRowPitch) = 0;
    virtual void UnmapStagingTexture(IStagingTexture* texture) = 0;

    // Buffer
    virtual BufferPtr CreateBuffer(const BufferDesc& bufferDesc) = 0;
    virtual void* MapBuffer(IBuffer* buffer, CpuAccessMode cpuAccessMode) = 0;
    virtual void UnmapBuffer(IBuffer* buffer) = 0;
    virtual MemoryRequirements GetBufferMemoryRequirements(IBuffer* buffer) = 0;
    virtual bool BindBufferMemory(IBuffer* buffer, IHeap* heap, uint64_t offset) = 0;
    virtual BufferPtr CreateHandleForNativeBuffer(ObjectType objectType, Object buffer, const BufferDesc& bufferDesc) = 0;

    // Shader
    virtual ShaderPtr CreateShader(const ShaderDesc& shaderDesc, const void* binary, size_t binarySize) = 0;
    virtual ShaderPtr CreateShaderSpecialization(IShader* baseShader, const ShaderSpecialization* constants, uint32_t numConstants) = 0;
    virtual ShaderLibraryPtr CreateShaderLibrary(const void* binary, size_t binarySize) = 0;

    // Sampler
    virtual SamplerPtr CreateSampler(const SamplerDesc& samplerDesc) = 0;

    // InputLayout
    virtual InputLayoutPtr CreateInputLayout(const VertexAttributeDesc* vertexAttributeDesc, uint32_t attributeCount, IShader* vertexShader) = 0;

    // Event
    virtual EventQueryPtr CreateEventQuery() = 0;
    virtual void SetEventQuery(IEventQuery* eventQuery, CommandQueue commandQueue) = 0;
    virtual bool PollEventQuery(IEventQuery* eventQuery) = 0;
    virtual void WaitEventQuery(IEventQuery* eventQuery) = 0;
    virtual void ResetEventQuery(IEventQuery* eventQuery) = 0;

    // Timer
    virtual TimerQueryPtr CreateTimerQuery() = 0;
    virtual bool PollTimerQuery(ITimerQuery* timeQuery) = 0;
    virtual float GetTimerQueryTime(ITimerQuery* timeQuery) = 0;
    virtual void ResetTimerQuery(ITimerQuery* timeQuery) = 0;

    // FrameBuffer
    virtual FrameBufferPtr CreateFrameBuffer(const FrameBufferDesc& frameBufferDesc) = 0;

    // GraphicsPipeline
    virtual GraphicsPipelinePtr CreateGraphicsPipeline(const GraphicsPipelineDesc& graphicsPipelineDesc, IFrameBuffer* frameBuffer) = 0;
    virtual GraphicsPipelinePtr CreateGraphicsPipelineFromCache(const PipelineCache& pipelineCache, GraphicsPipelineDesc& outDesc) = 0;
    virtual ComputePipelinePtr CreateComputePipeline(const ComputePipelineDesc& computePipelineDesc) = 0;
    virtual ComputePipelinePtr CreateComputePipelineFromCache(const PipelineCache& pipelineCache, ComputePipelineDesc& outDesc) = 0;
    virtual MeshletPipelinePtr CreateMeshletPipeline(const MeshletPipelineDesc& meshletPipelineDesc, IFrameBuffer* frameBuffer) = 0;
    virtual rt::PipelinePtr CreateRayTracingPipeline(const rt::PipelineDesc& pipelineDesc) = 0;

    // BindingLayout / DescriptorHeap
    virtual BindingLayoutPtr CreateBindingLayout(const BindingLayoutDesc& bindingLayoutDesc) = 0;
    virtual BindingLayoutPtr CreateBindlessLayout(const BindlessLayoutDesc& bindlessLayoutDesc) = 0;
    virtual BindingSetPtr CreateBindingSet(const BindingSetDesc& bindingSetDesc, IBindingLayout* bindingLayout) = 0;
    virtual DescriptorTablePtr CreateDescriptorTable(IBindingLayout* bindingLayout) = 0;
    virtual void ResizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents = true) = 0;
    virtual bool WriteDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& bindingItem) = 0;

    // BVH
    virtual rt::OpacityMicromapPtr CreateOpacityMicromap(const rt::OpacityMicromapDesc& opacityMicromapDesc) = 0;
    virtual rt::AccelStructPtr CreateAccelStruct(const rt::AccelStructDesc& accelStructDesc) = 0;
    virtual MemoryRequirements GetAccelStructMemoryRequirements(rt::IAccelStruct* accelStruct) = 0;
    virtual bool BindAccelStructMemory(rt::IAccelStruct* accelStruct, IHeap* heap, uint64_t offset) = 0;

    // CommandList
    virtual CommandListPtr CreateCommandList(const CommandListParameters& params = CommandListParameters()) = 0;
    virtual uint64_t ExecuteCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue = CommandQueue::Graphics) = 0;
    virtual void QueueWaitForCommandList(CommandQueue waitQueue, CommandQueue executionQueue, uint64_t instance) = 0;
    virtual void WaitForIdle() = 0;
    virtual void RunGarbageCollection() = 0;

    // Misc
    virtual bool QueryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) = 0;
    virtual FormatSupportBitset QueryFormatSupport(FormatType format) = 0;
    virtual Object GetNativeQueue(ObjectType objectType, CommandQueue queue) = 0;
    virtual GraphicsApiType GetGraphicsAPI() = 0;

    uint64_t ExecuteCommandList(ICommandList* commandList, CommandQueue executionQueue = CommandQueue::Graphics)
    {
        return ExecuteCommandLists(&commandList, 1, executionQueue);
    }

};
using DevicePtr = core::TReferencePtr<IDevice>;
}
