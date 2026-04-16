#include "render/pipeline/ComputePipeline.hpp"
#include "render/context/DrawContext.hpp"

namespace slug::render
{

uint64_t CreatePipelineKey(const rhi::ComputePipelineDesc& desc)
{
    uint64_t key = 0;
    rhi::HashCombine(key, desc.cs->GetDesc().debugName);
    rhi::HashCombine(key, desc.bindingLayouts.size());
    return key;
}

ComputePipeline* ComputePipeline::Create(DrawContext& context, PipelineCache& cacheData)
{
    ComputePipelinePtr pipeline = nullptr;
    auto service = context.GetResourceService();
    if (service->GetComputePipelinePool()->Get(cacheData.key, pipeline))
    {
        return pipeline;
    }

    rhi::ComputePipelineDesc desc = {};
    rhi::PipelineCache pipelineCache = {};
    pipelineCache.cache = cacheData.data.data();
    pipelineCache.cacheName = cacheData.name.c_str();
    pipelineCache.cacheSize = cacheData.data.size();

    pipeline.reset(new ComputePipeline);
    pipeline->m_pipeline = context.GetDevice()->CreateComputePipelineFromCache(pipelineCache, desc);
    service->GetComputePipelinePool()->Add(cacheData.key, pipeline);
    return pipeline;
}

ComputePipeline* ComputePipeline::Create(DrawContext& context, const PipelineInfo::ComputeProcessSettings& computeProcessSettings, const rhi::BindingLayoutPtr& bindingLayout)
{
    rhi::ComputePipelineDesc desc = {};
    desc.bindingLayouts.push_back(bindingLayout);
    computeProcessSettings.shader->GetComputeShader(desc.cs);

    uint64_t key = CreatePipelineKey(desc);
    ComputePipelinePtr pipeline = nullptr;
    auto service = context.GetResourceService();
    if (service->GetComputePipelinePool()->Get(key, pipeline))
    {
        return pipeline;
    }

    pipeline.reset(new ComputePipeline());
    pipeline->m_pipeline = context.GetDevice()->CreateComputePipeline(desc);
    service->GetComputePipelinePool()->Add(key, pipeline);
    return pipeline;
}

const rhi::ComputePipelinePtr& ComputePipeline::GetPipeline() const
{
    return m_pipeline;
}

}
