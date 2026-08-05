#include "render_core/pass/ComputePass.hpp"
#include "render_core/utility/ScopedMarker.hpp"
#include "render_core/service/RenderService.hpp"
#include "render_core/pipeline/GraphicsPipeline.hpp"

namespace slug::render_core
{

void ComputePass::Setup(const SetupParam& setup)
{

}

void ComputePass::Prepare(const PipelineInfo& pipelineInfo)
{

}

void ComputePass::Draw(RenderService& service, const PipelineInfo& pipelineInfo)
{
    ScopedMarker pipelineScoped(pipelineInfo.commandList, pipelineInfo.pipelineName);

    BindingCachePtr bindingCache;
    GetBinding(service, pipelineInfo, bindingCache);

    rhi::ComputeState state = {};
    auto& settings = pipelineInfo.computeProcessSettings;
    SetupComputePipeline(service, settings, bindingCache->GetBindingLayout(service), state);
    state.bindings = { bindingCache->GetBindingSet(service, pipelineInfo.computeProcessSettings.shader)};
    pipelineInfo.commandList->SetComputeState(state);
    pipelineInfo.commandList->Dispatch(settings.groupX, settings.groupY, settings.groupZ);
}

void ComputePass::GetBinding(RenderService& service, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache)
{

}

void ComputePass::SetupComputePipeline(RenderService& service, const PipelineInfo::ComputeProcessSettings& computeProcessSettings, const rhi::BindingLayoutPtr& bindingLayout, rhi::ComputeState& state)
{
    state.pipeline = ComputePipeline::Create(service, computeProcessSettings, bindingLayout)->GetPipeline();
}

}
