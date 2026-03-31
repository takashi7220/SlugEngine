#pragma once

#include "render/material/Material.hpp"
#include "render/pipeline/PipelineInfo.hpp"
#include "render/pipeline/PipelineCache.hpp"
#include "rhi/Rhi.hpp"



namespace slug
{
namespace render
{

class DrawContext;

class GraphicsPipeline : public core::ReferenceObject
{
public:
    static GraphicsPipeline* Create(DrawContext& context, PipelineCache& cacheData);
    static GraphicsPipeline* Create(
        DrawContext& context,
        const MaterialPtr& material,
        const PipelineInfo::DrawSettings& pipelineDrawSettings,
        const DrawObject::Settings& drawObjectDrawSettings,
        const rhi::GraphicsState& state,
        const core::TVector<rhi::IBindingLayout*> bindingLayouts
    );

    static GraphicsPipeline* Create(
        DrawContext& context,
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
    static void SetupVertexAttribute(DrawContext& context, const rhi::GraphicsState& state, rhi::GraphicsPipelineDesc& desc);
    static void SetupRasterState(const MaterialPtr& material, const PipelineInfo::DrawSettings& drawSettings, rhi::GraphicsPipelineDesc& desc);
    static void SetupDepth(const PipelineInfo::DrawSettings& drawSettings, rhi::GraphicsPipelineDesc& desc);
    static void SetupStencil(const PipelineInfo::DrawSettings& pipelineDrawSettings, uint8_t stencilValue, rhi::GraphicsPipelineDesc& desc);

private:
    rhi::GraphicsPipelinePtr m_pipeline;
    core::FixedString<32> m_name;
    uint64_t m_key = 0;
};
using GraphicsPipelinePtr = core::TReferencePtr<GraphicsPipeline>;


}
}
