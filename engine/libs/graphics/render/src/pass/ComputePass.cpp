#include "render/pass/ComputePass.hpp"
#include "render/utility/ScopedMarker.hpp"
#include "render/context/DrawContext.hpp"
#include "render/pipeline/GraphicsPipeline.hpp"

namespace slug::render
{

void ComputePass::Setup(const SetupParam& setup)
{

}

void ComputePass::Prepare(const PipelineInfo& pipelineInfo)
{

}

void ComputePass::Draw(DrawContext& context, const PipelineInfo& pipelineInfo)
{
    ScopedMarker pipelineScoped(pipelineInfo.commandList, pipelineInfo.pipelineName);

    BindingCachePtr bindingCache;
    GetBinding(context, pipelineInfo, bindingCache);

    rhi::ComputeState state = {};
    auto& settings = pipelineInfo.computeProcessSettings;
    SetupComputePipeline(context, settings, bindingCache->GetBindingLayout(context), state);
    state.bindings = { bindingCache->GetBindingSet(context, pipelineInfo.computeProcessSettings.shader)};
    pipelineInfo.commandList->SetComputeState(state);
    pipelineInfo.commandList->Dispatch(settings.groupX, settings.groupY, settings.groupZ);
}

void ComputePass::GetBinding(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache)
{

}

void ComputePass::SetupComputePipeline(DrawContext& context, const PipelineInfo::ComputeProcessSettings& computeProcessSettings, const rhi::BindingLayoutPtr& bindingLayout, rhi::ComputeState& state)
{
    state.pipeline = ComputePipeline::Create(context, computeProcessSettings, bindingLayout)->GetPipeline();
}

}
