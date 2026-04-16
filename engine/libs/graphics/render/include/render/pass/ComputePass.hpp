#pragma once

#include "render/pass/IRenderPass.hpp"

namespace slug::render
{

class ComputePass : public IRenderPass
{
public:
    virtual void Setup(const SetupParam& setup) override;
    virtual void Prepare(const PipelineInfo& pipelineInfo) override;
    virtual void Draw(DrawContext& context, const PipelineInfo& pipelineInfo) override;
    virtual void GetBinding(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache) override;

protected:
    void SetupComputePipeline(DrawContext& context, const PipelineInfo::ComputeProcessSettings& computeProcessSettings, const rhi::BindingLayoutPtr& bindingLayout, rhi::ComputeState& state);

};
using ComputePassPtr = core::TReferencePtr<ComputePass>;

}
