#include "render/context/DrawContext.hpp"
#include "render/graph/FrameGraph.hpp"
#include "render/pass/SurfacePass.hpp"
#include "render/pass/PostprocessPass.hpp"
#include "render/pass/ComputePass.hpp"
#include "render/texture/Framebuffer.hpp"

namespace slug::render
{

void FrameGraph::AddPass(DrawContext& context, const SurfacePassParam& param)
{
    SurfacePassPtr pass(new SurfacePass());
    pass->Initialize({ param.setupFunc, param.prepareFunc, param.drawFunc, param.bindingFunc });

    IRenderPass::SetupParam setup = {};
    pass->Setup(setup);

    GraphNodePtr node(new GraphNode());
    node->pass.reset(pass.get());
    node->pipelineInfo.surfaceSettings = param.surfaceSettings;
    node->pipelineInfo.drawSettings = param.settings;
    node->pipelineInfo.pipelineName = param.name.GetStr();
    node->pipelineInfo.view = param.view;
    node->reads = param.reads;
    node->writes = param.writes;
    m_graphNodes.push_back(node);    
}

void FrameGraph::AddPass(DrawContext& context, const PostprocessPassParam& param)
{
    PostProcessPassPtr pass(new PostProcessPass());
    pass->Initialize({ param.setupFunc, param.prepareFunc, param.drawFunc, param.bindingFunc });

    IRenderPass::SetupParam setup = {};
    pass->Setup(setup);

    GraphNodePtr node(new GraphNode());
    node->pass.reset(pass.get());
    node->pipelineInfo.postprocessSettings = param.postprocessSettings;
    node->pipelineInfo.drawSettings = param.settings;
    node->pipelineInfo.pipelineName = param.name.GetStr();
    node->pipelineInfo.view = param.view;
    node->reads = param.reads;
    node->writes = param.writes;
    m_graphNodes.push_back(node);
}

void FrameGraph::AddPass(DrawContext& context, const ComputeProcessPassParam& param)
{
    ComputePassPtr pass(new ComputePass());
    pass->Initialize({ param.setupFunc, param.prepareFunc, param.drawFunc, param.bindingFunc });

    IRenderPass::SetupParam setup = {};
    pass->Setup(setup);

    GraphNodePtr node(new GraphNode());
    node->pass.reset(pass.get());
    node->pipelineInfo.computeProcessSettings = param.computeProcessSettings;
    node->pipelineInfo.pipelineName = param.name.GetStr();
    node->reads = param.reads;
    node->writes = param.writes;
    m_graphNodes.push_back(node);
}

void FrameGraph::Compile(DrawContext& context)
{
    for (size_t i = 0; i < m_graphNodes.size(); i++)
    {
        auto& node = m_graphNodes.at(i);
        node->nexts.push_back((int32_t)i + 1);
        SetupRenderTarget(context, node->pipelineInfo, node->reads, node->writes);
    }
    m_graphNodes.back()->nexts.at(0) = -1;
}

void FrameGraph::Execute(DrawContext& context)
{
    GraphNodePtr& currentGraph = m_graphNodes.at(m_topNodeIndex);
    while (currentGraph != nullptr)
    {
        currentGraph->pass->Prepare(currentGraph->pipelineInfo);
        currentGraph->pass->Draw(context, currentGraph->pipelineInfo);
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

void FrameGraph::SetupRenderTarget(DrawContext& context, PipelineInfo& pipelineInfo, const core::TVector<RenderTargetParam>& reads, const core::TVector<RenderTargetParam>& writes)
{
    for (auto& read : reads)
    {
        Texture::SetupParameter textureParam;
        textureParam.textureInfo = read.textureInfo;
        pipelineInfo.readTextures.push_back(Texture::Create(context, textureParam));
    }

    pipelineInfo.framebuffer.reset(new Framebuffer());
    for (auto& write : writes)
    {
        Texture::SetupParameter textureParam;
        textureParam.textureInfo = write.textureInfo;
        if (write.type == RenderTargetType::Color)
        {
            pipelineInfo.framebuffer->renderTargets.push_back(Texture::Create(context, textureParam));
        }
        else if (write.type == RenderTargetType::Depth)
        {
            pipelineInfo.framebuffer->depthTarget = Texture::Create(context, textureParam);
        }
        else if (write.type == RenderTargetType::ShadingRateSurface)
        {
            pipelineInfo.framebuffer->shadingRateSurface = Texture::Create(context, textureParam);
        }
    }
}

}
