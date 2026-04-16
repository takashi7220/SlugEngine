#pragma once

#include "render/pass/IRenderPass.hpp"
#include "render/material/Material.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::render
{

class DrawContext;

class FrameGraph : public core::IService
{
public:
    SLUG_SERVICE_REGISTER_DEPENDENTS(FrameGraph, TestServiceA)

    enum RenderTargetType
    {
        Color,
        Depth,
        ShadingRateSurface
    };

    struct RenderTargetParam
    {
        TextureInfo textureInfo;
        RenderTargetType type;
    };

    struct SurfacePassParam
    {
        core::NameID name;
        ViewPtr view;
        PipelineInfo::DrawSettings settings;
        PipelineInfo::SurfaceSettings surfaceSettings;
        core::TVector<RenderTargetParam> reads;
        core::TVector<RenderTargetParam> writes;
        core::TFunctionObject<void(const IRenderPass::SetupParam&)> setupFunc;
        core::TFunctionObject<void(const PipelineInfo&)> prepareFunc;
        core::TFunctionObject<void(DrawContext& context, const PipelineInfo& pipelineInfo)> drawFunc;
        core::TFunctionObject<void(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache)> bindingFunc;
    };

    struct PostprocessPassParam
    {
        core::NameID name;
        PipelineInfo::DrawSettings settings;
        PipelineInfo::PostProcessSettings postprocessSettings;
        core::TVector<RenderTargetParam> reads;
        core::TVector<RenderTargetParam> writes;
        core::TFunctionObject<void(const IRenderPass::SetupParam&)> setupFunc;
        core::TFunctionObject<void(const PipelineInfo&)> prepareFunc;
        core::TFunctionObject<void(DrawContext& context, const PipelineInfo& pipelineInfo)> drawFunc;
        core::TFunctionObject<void(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache)> bindingFunc;
    };

    struct ComputeProcessPassParam
    {
        core::NameID name;
        PipelineInfo::ComputeProcessSettings computeProcessSettings;
        core::TVector<RenderTargetParam> reads;
        core::TVector<RenderTargetParam> writes;
        core::TFunctionObject<void(const IRenderPass::SetupParam&)> setupFunc;
        core::TFunctionObject<void(const PipelineInfo&)> prepareFunc;
        core::TFunctionObject<void(DrawContext& context, const PipelineInfo& pipelineInfo)> drawFunc;
        core::TFunctionObject<void(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache)> bindingFunc;
    };
public:
    void AddPass(DrawContext& context, const SurfacePassParam& param);
    void AddPass(DrawContext& context, const PostprocessPassParam& param);
    void AddPass(DrawContext& context, const ComputeProcessPassParam& param);
    void Compile(DrawContext& context);
    void Execute(DrawContext& context);

private:
    struct GraphNode : public core::ReferenceObject
    {
        IRenderPassPtr pass;
        PipelineInfo pipelineInfo;
        core::TVector<RenderTargetParam> reads;
        core::TVector<RenderTargetParam> writes;
        core::TVector<int32_t> nexts;
    };
    using GraphNodePtr = core::TReferencePtr<GraphNode>;

    void SetupRenderTarget(DrawContext& context, PipelineInfo& pipelineInfo, const core::TVector<RenderTargetParam>& reads, const core::TVector<RenderTargetParam>& writes);

private:
    uint32_t m_topNodeIndex = 0;
    core::TVector<GraphNodePtr> m_graphNodes;
};
using FrameGraphPtr = core::TReferencePtr<FrameGraph>;

}
