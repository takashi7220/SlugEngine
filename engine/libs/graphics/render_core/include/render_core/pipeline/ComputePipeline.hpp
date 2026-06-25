#pragma once


#include "rhi/Rhi.hpp"
#include "render_core/pipeline/PipelineCache.hpp"
#include "render_core/pipeline/PipelineInfo.hpp"

namespace slug::render_core
{

class DrawContext;

class ComputePipeline;
using ComputePipelinePtr = core::TReferencePtr<ComputePipeline>;


class ComputePipeline : public core::ReferenceObject
{
public:
    static ComputePipelinePtr Create(RenderService& service, PipelineCache& cacheData);
    static ComputePipelinePtr Create(RenderService& service, const PipelineInfo::ComputeProcessSettings& computeProcessSettings, const rhi::BindingLayoutPtr& bindingLayout);

    PipelineCache GetCache() const;

    const rhi::ComputePipelinePtr& GetPipeline() const;

private:
    rhi::ComputePipelinePtr m_pipeline;
};

}
