#include "rhi/d3d12/resource/ComputePipeline.hpp"
#include "rhi/common/Resource.hpp"

namespace slug::rhi::d3d12
{

const ComputePipelineDesc& ComputePipeline::GetDesc() const
{
    return desc;
}

Object ComputePipeline::GetNativeObject(ObjectType objectType)
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
