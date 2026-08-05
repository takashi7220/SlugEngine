#pragma once

#include "core/pattern/ResourcePool.hpp"
#include "render_core/pipeline/GraphicsPipeline.hpp"

namespace slug::render_core
{

class GraphicsPipelinePool : public core::ResourcePool<uint64_t, GraphicsPipelinePtr>
{
public:
private:
};
using GraphicsPipelinePoolPtr = core::TReferencePtr<GraphicsPipelinePool>;

}
