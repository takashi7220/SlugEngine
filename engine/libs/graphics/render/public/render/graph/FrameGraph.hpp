#pragma once

#include "render_core/pass/IRenderPass.hpp"
#include "render_core/material/Material.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::render
{
class RenderService;

class FrameGraph
{
public:
    enum RenderTargetType
    {
        Color,
        Depth,
        ShadingRateSurface
    };

    struct RenderTargetParam
    {
        render_core::TextureInfo textureInfo;
        RenderTargetType type;
    };

    struct SurfacePassParam
    {
        core::FixedString<64> name;
        render_core::ViewPtr view;
        render_core::PipelineInfo::DrawSettings settings;
        render_core::PipelineInfo::SurfaceSettings surfaceSettings;
        core::TVector<RenderTargetParam> reads;
        core::TVector<RenderTargetParam> writes;
        core::TFunctionObject<void(const render_core::IRenderPass::SetupParam&)> setupFunc;
        core::TFunctionObject<void(const render_core::PipelineInfo&)> prepareFunc;
        core::TFunctionObject<void(render_core::RenderService& service, const render_core::PipelineInfo& pipelineInfo)> drawFunc;
        core::TFunctionObject<void(render_core::RenderService& service, const render_core::PipelineInfo& pipelineInfo, render_core::BindingCachePtr& bindingCache)> bindingFunc;
    };

    struct PostprocessPassParam
    {
        core::FixedString<64> name;
        render_core::ViewPtr view;
        render_core::PipelineInfo::DrawSettings settings;
        render_core::PipelineInfo::PostProcessSettings postprocessSettings;
        core::TVector<RenderTargetParam> reads;
        core::TVector<RenderTargetParam> writes;
        core::TFunctionObject<void(const render_core::IRenderPass::SetupParam&)> setupFunc;
        core::TFunctionObject<void(const render_core::PipelineInfo&)> prepareFunc;
        core::TFunctionObject<void(render_core::RenderService& service, const render_core::PipelineInfo& pipelineInfo)> drawFunc;
        core::TFunctionObject<void(render_core::RenderService& service, const render_core::PipelineInfo& pipelineInfo, render_core::BindingCachePtr& bindingCache)> bindingFunc;
    };

    struct ComputeProcessPassParam
    {
        core::FixedString<64> name;
        render_core::PipelineInfo::ComputeProcessSettings computeProcessSettings;
        core::TVector<RenderTargetParam> reads;
        core::TVector<RenderTargetParam> writes;
        core::TFunctionObject<void(const render_core::IRenderPass::SetupParam&)> setupFunc;
        core::TFunctionObject<void(const render_core::PipelineInfo&)> prepareFunc;
        core::TFunctionObject<void(render_core::RenderService& service, const render_core::PipelineInfo& pipelineInfo)> drawFunc;
        core::TFunctionObject<void(render_core::RenderService& service, const render_core::PipelineInfo& pipelineInfo, render_core::BindingCachePtr& bindingCache)> bindingFunc;
    };
public:
    void AddPass(render_core::RenderService& service, const SurfacePassParam& param);
    void AddPass(render_core::RenderService& service, const PostprocessPassParam& param);
    void AddPass(render_core::RenderService& service, const ComputeProcessPassParam& param);
    void Compile(render_core::RenderService& service);
    void Execute(render_core::RenderService& service);

private:
    struct GraphNode : public core::ReferenceObject
    {
        render_core::IRenderPassPtr pass;
        render_core::PipelineInfo pipelineInfo;
        core::TVector<RenderTargetParam> reads;
        core::TVector<RenderTargetParam> writes;
        core::TVector<int32_t> nexts;
    };
    using GraphNodePtr = core::TReferencePtr<GraphNode>;

    void SetupRenderTarget(render_core::RenderService& service, render_core::PipelineInfo& pipelineInfo, const core::TVector<RenderTargetParam>& reads, const core::TVector<RenderTargetParam>& writes);

private:
    uint32_t m_topNodeIndex = 0;
    core::TVector<GraphNodePtr> m_graphNodes;
};
}
