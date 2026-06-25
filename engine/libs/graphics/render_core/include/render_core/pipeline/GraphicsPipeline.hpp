#pragma once

#include "render_core/material/Material.hpp"
#include "render_core/pipeline/PipelineInfo.hpp"
#include "render_core/pipeline/PipelineCache.hpp"
#include "rhi/Rhi.hpp"



namespace slug::render_core
{

class RenderService;
class GraphicsPipeline;

using GraphicsPipelinePtr = core::TReferencePtr<GraphicsPipeline>;

class GraphicsPipeline : public core::ReferenceObject
{
public:
    static GraphicsPipelinePtr Create(RenderService& service, PipelineCache& cacheData);
    static GraphicsPipelinePtr Create(
        RenderService& service,
        const MaterialPtr& material,
        const PipelineInfo::DrawSettings& pipelineDrawSettings,
        const DrawObject::Settings& drawObjectDrawSettings,
        const rhi::GraphicsState& state,
        const core::TVector<rhi::IBindingLayout*> bindingLayouts
    );

    static GraphicsPipelinePtr Create(
        RenderService& service,
        const PipelineInfo::DrawSettings& pipelineDrawSettings,
        const PipelineInfo::PostProcessSettings& postProcessSettings,
        const rhi::GraphicsState& state,
        const rhi::BindingLayoutPtr& bindingLayout
    );

    PipelineCache GetCache() const;
    rhi::GraphicsPipelinePtr& GetPipeline();

private:
    static void SetupShader(const MaterialPtr& material, rhi::GraphicsPipelineDesc& desc);
    static void SetupBindingLayouts(const core::TVector<rhi::IBindingLayout*> bindingLayouts, rhi::GraphicsPipelineDesc& desc);
    static void SetupVertexAttribute(RenderService& service, const rhi::GraphicsState& state, rhi::GraphicsPipelineDesc& desc);
    static void SetupRasterState(const MaterialPtr& material, const PipelineInfo::DrawSettings& drawSettings, rhi::GraphicsPipelineDesc& desc);
    static void SetupDepth(const PipelineInfo::DrawSettings& drawSettings, rhi::GraphicsPipelineDesc& desc);
    static void SetupStencil(const PipelineInfo::DrawSettings& pipelineDrawSettings, uint8_t stencilValue, rhi::GraphicsPipelineDesc& desc);

private:
    rhi::GraphicsPipelinePtr m_pipeline;
    core::FixedString<32> m_name;
    uint64_t m_key = 0;
};

}
