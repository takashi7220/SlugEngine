#pragma once

#include "core/pattern/ResourcePool.hpp"
#include "render_core/pipeline/ComputePipeline.hpp"

namespace slug::render_core
{

class ComputePipelinePool : public core::ResourcePool<uint64_t, ComputePipelinePtr>
{
public:
private:
};
using ComputePipelinePoolPtr = core::TReferencePtr<ComputePipelinePool>;

}
