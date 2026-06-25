#pragma once

#include "rhi/interface/IComputePipiline.hpp"
#include "rhi/ResourceType.hpp"
#include "rhi/d3d12/resource/RootSignature.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"

namespace slug::rhi::d3d12
{

class ComputePipeline : public IComputePipeline
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(ComputePipeline, IComputePipeline)
    const ComputePipelineDesc& GetDesc() const override;
    Object GetNativeObject(ObjectType objectType) override;

public:
    ComputePipelineDesc desc;
    core::TReferencePtr<RootSignature> rootSignature;
    core::TReferencePtr<ID3D12PipelineState> pipelineState;
};

}
