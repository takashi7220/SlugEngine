#include "rhi/d3d12/resource/MeshletPipeline.hpp"
#include "rhi/common/Resource.hpp"

namespace slug::rhi::d3d12
{
const MeshletPipelineDesc& MeshletPipeline::GetDesc() const
{
    return desc;
}

const FrameBufferInfo& MeshletPipeline::GetFrameBufferInfo() const
{
    return framebufferInfo;
}

Object MeshletPipeline::GetNativeObject(ObjectType objectType)
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
