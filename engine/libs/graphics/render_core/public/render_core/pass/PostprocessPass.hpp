#pragma once

#include "render_core/pass/IRenderPass.hpp"

namespace slug::render_core
{

class PostProcessPass : public IRenderPass
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(PostProcessPass, IRenderPass)

    virtual void Setup(const SetupParam& setup) override;
    virtual void Prepare(const PipelineInfo& pipelineInfo) override;
    virtual void Draw(RenderService& service, const PipelineInfo& pipelineInfo) override;
    virtual void GetBinding(RenderService& service, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache) override;

protected:
    void SetupGraphicsPipeline(RenderService& service, const PipelineInfo::DrawSettings& pipelineDrawSettings, const PipelineInfo::PostProcessSettings& postProcessSettings, const rhi::BindingLayoutPtr& bindingLayout, rhi::GraphicsState& state);

};
using PostProcessPassPtr = core::TReferencePtr<PostProcessPass>;

}
