#include "render_core/service/RenderService.hpp"
#include "render_core/pass/SurfacePass.hpp"
#include "render_core/pass/PostprocessPass.hpp"
#include "render_core/pass/ComputePass.hpp"
#include "render_core/texture/Framebuffer.hpp"
#include "render/graph/FrameGraph.hpp"

namespace slug::render
{
void FrameGraph::AddPass(render_core::RenderService& service, const SurfacePassParam& param)
{
    render_core::SurfacePassPtr pass = core::NewObject<render_core::SurfacePass>();
    pass->Initialize({ param.setupFunc, param.prepareFunc, param.drawFunc, param.bindingFunc });

    render_core::IRenderPass::SetupParam setup = {};
    pass->Setup(setup);

    GraphNodePtr node = core::NewObject<GraphNode>();
    node->pass.reset(pass.get());
    node->pipelineInfo.surfaceSettings = param.surfaceSettings;
    node->pipelineInfo.drawSettings = param.settings;
    node->pipelineInfo.pipelineName = param.name.c_str();
    node->pipelineInfo.view = param.view;
    node->reads = param.reads;
    node->writes = param.writes;
    m_graphNodes.push_back(node);    
}

void FrameGraph::AddPass(render_core::RenderService& service, const PostprocessPassParam& param)
{
    render_core::PostProcessPassPtr pass = core::NewObject<render_core::PostProcessPass>();
    pass->Initialize({ param.setupFunc, param.prepareFunc, param.drawFunc, param.bindingFunc });

    render_core::IRenderPass::SetupParam setup = {};
    pass->Setup(setup);

    GraphNodePtr node = core::NewObject<GraphNode>();
    node->pass.reset(pass.get());
    node->pipelineInfo.postprocessSettings = param.postprocessSettings;
    node->pipelineInfo.drawSettings = param.settings;
    node->pipelineInfo.pipelineName = param.name.c_str();
    node->pipelineInfo.view = param.view;
    node->reads = param.reads;
    node->writes = param.writes;
    m_graphNodes.push_back(node);
}

void FrameGraph::AddPass(render_core::RenderService& service, const ComputeProcessPassParam& param)
{
    render_core::ComputePassPtr pass = core::NewObject<render_core::ComputePass>();
    pass->Initialize({ param.setupFunc, param.prepareFunc, param.drawFunc, param.bindingFunc });

    render_core::IRenderPass::SetupParam setup = {};
    pass->Setup(setup);

    GraphNodePtr node = core::NewObject<GraphNode>();
    node->pass.reset(pass.get());
    node->pipelineInfo.computeProcessSettings = param.computeProcessSettings;
    node->pipelineInfo.pipelineName = param.name.c_str();
    node->reads = param.reads;
    node->writes = param.writes;
    m_graphNodes.push_back(node);
}

void FrameGraph::Compile(render_core::RenderService& service)
{
    for (size_t i = 0; i < m_graphNodes.size(); i++)
    {
        auto& node = m_graphNodes.at(i);
        node->nexts.push_back((int32_t)i + 1);
        SetupRenderTarget(service, node->pipelineInfo, node->reads, node->writes);
    }
    m_graphNodes.back()->nexts.at(0) = -1;
}

void FrameGraph::Execute(render_core::RenderService& service)
{
    GraphNodePtr& currentGraph = m_graphNodes.at(m_topNodeIndex);
    while (currentGraph != nullptr)
    {
        currentGraph->pass->Prepare(currentGraph->pipelineInfo);
        currentGraph->pass->Draw(service, currentGraph->pipelineInfo);
        int32_t nextIndex = currentGraph->nexts.at(0);
        if (nextIndex < m_graphNodes.size() && nextIndex >= 0)
        {
            currentGraph = m_graphNodes.at(nextIndex);
        }
        else
        {
            currentGraph = nullptr;
        }
    }
}

void FrameGraph::SetupRenderTarget(render_core::RenderService& service, render_core::PipelineInfo& pipelineInfo, const core::TVector<RenderTargetParam>& reads, const core::TVector<RenderTargetParam>& writes)
{
    for (auto& read : reads)
    {
        render_core::Texture::SetupParameter textureParam;
        textureParam.textureInfo = read.textureInfo;
        pipelineInfo.readTextures.push_back(render_core::Texture::Create(service, textureParam));
    }

    pipelineInfo.framebuffer.reset(new render_core::Framebuffer());
    for (auto& write : writes)
    {
        render_core::Texture::SetupParameter textureParam;
        textureParam.textureInfo = write.textureInfo;
        if (write.type == RenderTargetType::Color)
        {
            pipelineInfo.framebuffer->renderTargets.push_back(render_core::Texture::Create(service, textureParam));
        }
        else if (write.type == RenderTargetType::Depth)
        {
            pipelineInfo.framebuffer->depthTarget = render_core::Texture::Create(service, textureParam);
        }
        else if (write.type == RenderTargetType::ShadingRateSurface)
        {
            pipelineInfo.framebuffer->shadingRateSurface = render_core::Texture::Create(service, textureParam);
        }
    }
}
}
