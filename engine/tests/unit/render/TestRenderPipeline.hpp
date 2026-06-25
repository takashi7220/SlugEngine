#pragma once

#include "render_core/service/RenderService.hpp"
#include "render/graph/FrameGraph.hpp"

namespace slug::render_unit
{

class TestRenderPipeline final
{
public:
    void Setup(render_core::RenderServicePtr& service);

private:
    render::FrameGraph m_frameGraph;
};
}
