#pragma once

#include "render_core/pass/IRenderPass.hpp"

namespace slug::render_core
{

class SurfacePass : public IRenderPass
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(SurfacePass, IRenderPass)

    virtual void Setup(const SetupParam& setup) override;
    virtual void Prepare(const PipelineInfo& pipelineInfo) override;
    virtual void Draw(RenderService& service, const PipelineInfo& pipelineInfo) override;
    virtual void GetBinding(RenderService& service, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache) override;

private:
    void FlushDraw(rhi::ICommandList* commandList, rhi::GraphicsState& state, rhi::DrawArguments& drawArg);
    void SetupGraphicsPipeline(
        RenderService& service,
        const PipelineInfo::DrawSettings& pipelineDrawSettings,
        const View& view,
        const DrawObject& drawObject,
        const MaterialPtr& material,
        rhi::GraphicsState& state);

    void SetupGeometry(const GeometroyAttributePtr& geometry, rhi::GraphicsState& state);

private:
};
using SurfacePassPtr = core::TReferencePtr<SurfacePass>;
}
