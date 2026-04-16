#pragma once

#include "core/pattern/ResourcePool.hpp"
#include "render/pipeline/ComputePipeline.hpp"

namespace slug::render
{

class ComputePipelinePool : public core::ResourcePool<uint64_t, ComputePipelinePtr>
{
public:
private:
};
using ComputePipelinePoolPtr = core::TReferencePtr<ComputePipelinePool>;

}
