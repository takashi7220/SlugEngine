#include "render_core/pass/PostprocessPass.hpp"
#include "render_core/utility/ScopedMarker.hpp"
#include "render_core/service/RenderService.hpp"
#include "render_core/pipeline/GraphicsPipeline.hpp"

namespace slug::render_core
{

void PostProcessPass::Setup(const SetupParam& setup)
{

}

void PostProcessPass::Prepare(const PipelineInfo& pipelineInfo)
{

}

void PostProcessPass::Draw(RenderService& service, const PipelineInfo& pipelineInfo)
{
    ScopedMarker pipelineScoped(pipelineInfo.commandList, pipelineInfo.pipelineName);

    BindingCachePtr bindingCache = {};
    GetBinding(service, pipelineInfo, bindingCache);

    rhi::GraphicsState state;
    SetupGraphicsPipeline(service, pipelineInfo.drawSettings, pipelineInfo.postprocessSettings, bindingCache->GetBindingLayout(service), state);
    state.framebuffer = pipelineInfo.framebuffer->GetFramebuffer(service, pipelineInfo.view);
    state.bindings = { bindingCache->GetBindingSet(service, pipelineInfo.postprocessSettings.shader)};
    state.viewport = pipelineInfo.view->GetViewportState();
    pipelineInfo.commandList->SetGraphicsState(state);
    rhi::DrawArguments args;
    args.instanceCount = 1;
    args.vertexCount = 4;
    pipelineInfo.commandList->Draw(args);
}

void PostProcessPass::GetBinding(RenderService& service, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache)
{

}

void PostProcessPass::SetupGraphicsPipeline(RenderService& service, const PipelineInfo::DrawSettings& pipelineDrawSettings, const PipelineInfo::PostProcessSettings& postProcessSettings, const rhi::BindingLayoutPtr& bindingLayout, rhi::GraphicsState& state)
{
    auto pipeline = GraphicsPipeline::Create(service, pipelineDrawSettings, postProcessSettings, state, bindingLayout);
    state.pipeline = pipeline->GetPipeline();
}

}
