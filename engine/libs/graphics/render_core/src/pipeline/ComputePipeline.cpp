#include "render_core/pipeline/ComputePipeline.hpp"
#include "render_core/service/RenderService.hpp"
#include "core/debug/CompileMessage.hpp"

namespace slug::render_core
{

uint64_t CreatePipelineKey(const rhi::ComputePipelineDesc& desc)
{
    uint64_t key = 0;
    rhi::HashCombine(key, desc.cs->GetDesc().debugName);
    rhi::HashCombine(key, desc.bindingLayouts.size());
    return key;
}

ComputePipelinePtr ComputePipeline::Create(RenderService& service, PipelineCache& cacheData)
{
    ComputePipelinePtr pipeline = nullptr;
    ComputePipelinePoolPtr& resourcePool = service.GetResourcePool().GetComputePipelinePool();
    if (resourcePool->Get(cacheData.key, pipeline))
    {
        return pipeline;
    }

    rhi::ComputePipelineDesc desc = {};
    rhi::PipelineCache pipelineCache = {};
    pipelineCache.cache = cacheData.data.data();
    pipelineCache.cacheName = cacheData.name.c_str();
    pipelineCache.cacheSize = cacheData.data.size();

    pipeline.reset(new ComputePipeline);
    pipeline->m_pipeline = service.GetDevice()->CreateComputePipelineFromCache(pipelineCache, desc);
    resourcePool->Add(cacheData.key, pipeline);
    return pipeline;
}

ComputePipelinePtr ComputePipeline::Create(RenderService& service, const PipelineInfo::ComputeProcessSettings& computeProcessSettings, const rhi::BindingLayoutPtr& bindingLayout)
{
    rhi::ComputePipelineDesc desc = {};
    desc.bindingLayouts.push_back(bindingLayout);
    computeProcessSettings.shader->GetComputeShader(desc.cs);

    uint64_t key = CreatePipelineKey(desc);
    ComputePipelinePtr pipeline = nullptr;
    ComputePipelinePoolPtr& resourcePool = service.GetResourcePool().GetComputePipelinePool();
    if (resourcePool->Get(key, pipeline))
    {
        return pipeline;
    }

    pipeline.reset(new ComputePipeline());
    pipeline->m_pipeline = service.GetDevice()->CreateComputePipeline(desc);
    resourcePool->Add(key, pipeline);
    return pipeline;
}

PipelineCache ComputePipeline::GetCache() const
{
    SLUG_COMPILE_TODO_MESSAGE("Not Implement CompitePipeline GetCache()");
#if 0
    PipelineCache cache;
    m_pipeline->GetCache(cache.data);
    cache.name = m_name;
    cache.key = m_key;
    return cache;
#endif
    return PipelineCache();
}

const rhi::ComputePipelinePtr& ComputePipeline::GetPipeline() const
{
    return m_pipeline;
}

}
