#include "render/pass/PostprocessPass.hpp"
#include "render/utility/ScopedMarker.hpp"
#include "render/context/DrawContext.hpp"
#include "render/pipeline/GraphicsPipeline.hpp"

namespace slug
{
namespace render
{

void PostProcessPass::Setup(const SetupParam& setup)
{

}

void PostProcessPass::Prepare(const PipelineInfo& pipelineInfo)
{

}

void PostProcessPass::Draw(DrawContext& context, const PipelineInfo& pipelineInfo)
{
    ScopedMarker pipelineScoped(pipelineInfo.commandList, pipelineInfo.pipelineName);

    BindingCachePtr bindingCache = {};
    GetBinding(context, pipelineInfo, bindingCache);

    rhi::GraphicsState state;
    SetupGraphicsPipeline(context, pipelineInfo.drawSettings, pipelineInfo.postprocessSettings, bindingCache->GetBindingLayout(context), state);
    state.framebuffer = pipelineInfo.framebuffer->GetFramebuffer(context, pipelineInfo.view);
    state.bindings = { bindingCache->GetBindingSet(context, pipelineInfo.postprocessSettings.shader)};
    state.viewport = pipelineInfo.view->GetViewportState();
    pipelineInfo.commandList->SetGraphicsState(state);
    rhi::DrawArguments args;
    args.instanceCount = 1;
    args.vertexCount = 4;
    pipelineInfo.commandList->Draw(args);
}

void PostProcessPass::GetBinding(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache)
{

}

void PostProcessPass::SetupGraphicsPipeline(DrawContext& context, const PipelineInfo::DrawSettings& pipelineDrawSettings, const PipelineInfo::PostProcessSettings& postProcessSettings, const rhi::BindingLayoutPtr& bindingLayout, rhi::GraphicsState& state)
{
    auto pipeline = GraphicsPipeline::Create(context, pipelineDrawSettings, postProcessSettings, state, bindingLayout);
    state.pipeline = pipeline->GetPipeline();
}

}
}
