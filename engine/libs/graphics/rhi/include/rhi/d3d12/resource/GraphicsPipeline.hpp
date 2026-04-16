#pragma once

#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/RootSignature.hpp"

namespace slug::rhi::d3d12
{
class GraphicsPipeline : public IGraphicsPipeline
{
public:
    const GraphicsPipelineDesc& GetDesc() const override;
    const FrameBufferInfo& GetFrameBufferInfo() const override;
    void GetCache(core::TVector<uint8_t>& cache) const override;
    Object GetNativeObject(ObjectType objectType) override;
public:
    GraphicsPipelineDesc desc = {};
    FrameBufferInfo framebufferInfo = {};
    core::TReferencePtr<RootSignature> rootSignature = nullptr;
    core::TReferencePtr<ID3D12PipelineState> pipelineState = nullptr;
    bool requiresBlendFactor = false;
};
}
