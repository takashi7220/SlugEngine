#pragma once

#include "core/pattern/ResourcePool.hpp"
#include "render/pipeline/GraphicsPipeline.hpp"

namespace slug
{
namespace render
{

class GraphicsPipelinePool : public core::ResourcePool<uint64_t, GraphicsPipelinePtr>
{
public:
private:
};
using GraphicsPipelinePoolPtr = core::TReferencePtr<GraphicsPipelinePool>;

}
}
