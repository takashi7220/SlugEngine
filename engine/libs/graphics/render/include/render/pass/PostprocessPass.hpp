#pragma once

#include "render/pass/IRenderPass.hpp"

namespace slug::render
{

class PostProcessPass : public IRenderPass
{
public:
    virtual void Setup(const SetupParam& setup) override;
    virtual void Prepare(const PipelineInfo& pipelineInfo) override;
    virtual void Draw(DrawContext& context, const PipelineInfo& pipelineInfo) override;
    virtual void GetBinding(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache) override;

protected:
    void SetupGraphicsPipeline(DrawContext& context, const PipelineInfo::DrawSettings& pipelineDrawSettings, const PipelineInfo::PostProcessSettings& postProcessSettings, const rhi::BindingLayoutPtr& bindingLayout, rhi::GraphicsState& state);

};
using PostProcessPassPtr = core::TReferencePtr<PostProcessPass>;

}
