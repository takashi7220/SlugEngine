#include "rhi/d3d12/resource/GraphicsPipeline.hpp"
#include "rhi/common/Resource.hpp"
#include "core/memory/MemoryUtilities.hpp"
namespace slug::rhi::d3d12
{

const GraphicsPipelineDesc& GraphicsPipeline::GetDesc() const
{
    return desc;
}

const FrameBufferInfo& GraphicsPipeline::GetFrameBufferInfo() const
{
    return framebufferInfo;
}

void GraphicsPipeline::GetCache(core::TVector<uint8_t>& cache) const
{
    ID3DBlob* blob = {};
    pipelineState->GetCachedBlob(&blob);
    if (blob != nullptr)
    {
        cache.resize(blob->GetBufferSize());
        core::MemoryUtilities::Memcpy(cache.data(), cache.size(), blob->GetBufferPointer(), blob->GetBufferSize());
    }
}

Object GraphicsPipeline::GetNativeObject(ObjectType objectType)
{
    switch (objectType)
    {
    case object_types::D3D12_RootSignature:
        return rootSignature->GetNativeObject(objectType);
    case object_types::D3D12_PipelineState:
        return Object(pipelineState.get());
    default:
        return nullptr;
    }
}

}
