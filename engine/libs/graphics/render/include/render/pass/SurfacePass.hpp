#pragma once

#include "render/pass/IRenderPass.hpp"

namespace slug::render
{

class SurfacePass : public IRenderPass
{
public:
    virtual void Setup(const SetupParam& setup) override;
    virtual void Prepare(const PipelineInfo& pipelineInfo) override;
    virtual void Draw(DrawContext& context, const PipelineInfo& pipelineInfo) override;
    virtual void GetBinding(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache) override;

private:
    void FlushDraw(rhi::ICommandList* commandList, rhi::GraphicsState& state, rhi::DrawArguments& drawArg);
    void SetupGraphicsPipeline(
        DrawContext& context,
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
