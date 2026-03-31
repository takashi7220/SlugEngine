#pragma once


#include "rhi/Rhi.hpp"
#include "render/pipeline/PipelineCache.hpp"
#include "render/pipeline/PipelineInfo.hpp"

namespace slug
{
namespace render
{

class DrawContext;

class ComputePipeline : public core::ReferenceObject
{
public:
    static ComputePipeline* Create(DrawContext& context, PipelineCache& cacheData);
    static ComputePipeline* Create(DrawContext& context, const PipelineInfo::ComputeProcessSettings& computeProcessSettings, const rhi::BindingLayoutPtr& bindingLayout);

    const rhi::ComputePipelinePtr& GetPipeline() const;

private:
    rhi::ComputePipelinePtr m_pipeline;
};
using ComputePipelinePtr = core::TReferencePtr<ComputePipeline>;

}
}
