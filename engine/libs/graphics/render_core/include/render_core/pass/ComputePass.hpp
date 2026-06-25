#pragma once

#include "render_core/pass/IRenderPass.hpp"

namespace slug::render_core
{

class ComputePass : public IRenderPass
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(ComputePass, IRenderPass)

    virtual void Setup(const SetupParam& setup) override;
    virtual void Prepare(const PipelineInfo& pipelineInfo) override;
    virtual void Draw(RenderService& service, const PipelineInfo& pipelineInfo) override;
    virtual void GetBinding(RenderService& service, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache) override;

protected:
    void SetupComputePipeline(RenderService& service, const PipelineInfo::ComputeProcessSettings& computeProcessSettings, const rhi::BindingLayoutPtr& bindingLayout, rhi::ComputeState& state);

};
using ComputePassPtr = core::TReferencePtr<ComputePass>;

}
