#pragma once

#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/D3D12Utility.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/RootSignature.hpp"

namespace slug::rhi::d3d12
{
class MeshletPipeline : public IMeshletPipeline
{
public:
    const MeshletPipelineDesc& GetDesc() const override;
    const FrameBufferInfo& GetFrameBufferInfo() const override;
    Object GetNativeObject(ObjectType objectType) override;

public:
    MeshletPipelineDesc desc;
    FrameBufferInfo framebufferInfo;
    core::TReferencePtr<RootSignature> rootSignature;
    core::TReferencePtr<ID3D12PipelineState> pipelineState;
    DX12ViewportState viewportState;
    bool requiresBlendFactor = false;
};


}
