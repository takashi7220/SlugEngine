#pragma once

#include "TestRenderPipeline.hpp"

namespace slug::render_unit
{

void TestRenderPipeline::Setup(render_core::RenderServicePtr& service)
{
    // Depth Pre-Pass
    {
        render::FrameGraph::SurfacePassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // GBuffer Pass
    {
        render::FrameGraph::SurfacePassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Shadow Map Pass
    {
        render::FrameGraph::SurfacePassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // SSAO Pass
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Deferred Lighting Pass
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Sky Pass
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Fog Pass
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Anti-Aliasing Pass
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // DOF
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Motion Blur
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Bloom
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Calculate Exposure
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // OIIO ColorGrading
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Debug 3D
    {
        render::FrameGraph::SurfacePassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // HUD
    {
        render::FrameGraph::SurfacePassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Debug HUD
    {
        render::FrameGraph::SurfacePassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Combine
    {
        render::FrameGraph::ComputeProcessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }

    // Present
    {
        render::FrameGraph::PostprocessPassParam param;
        m_frameGraph.AddPass(*service, param);
    }
}
}
