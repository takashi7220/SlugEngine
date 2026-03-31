#include "render/pipeline/GraphicsPipeline.hpp"
#include "render/utility/Utility.hpp"
#include "render/context/DrawObject.hpp"
#include "render/context/DrawContext.hpp"

namespace slug
{
namespace render
{

uint64_t CreatePipelineKey(const rhi::GraphicsPipelineDesc& desc)
{
    uint64_t key = 0;
    rhi::HashCombine(key, desc.inputLayout->GetNumAttributes());
    if (desc.vs != nullptr)
    {
        rhi::HashCombine(key, desc.vs->GetDesc().debugName);
    }
    if (desc.ps != nullptr)
    {
        rhi::HashCombine(key, desc.ps->GetDesc().debugName);
    }
    if (desc.ds != nullptr)
    {
        rhi::HashCombine(key, desc.ds->GetDesc().debugName);
    }
    if (desc.hs != nullptr)
    {
        rhi::HashCombine(key, desc.hs->GetDesc().debugName);
    }
    if (desc.gs != nullptr)
    {
        rhi::HashCombine(key, desc.gs->GetDesc().debugName);
    }

    rhi::HashCombine(key, desc.renderState.depthStencilState.depthFunc);
    rhi::HashCombine(key, desc.renderState.depthStencilState.depthTestEnable);
    rhi::HashCombine(key, desc.renderState.depthStencilState.depthWriteEnable);
    rhi::HashCombine(key, desc.renderState.depthStencilState.backFaceStencil.depthFailOp);
    rhi::HashCombine(key, desc.renderState.depthStencilState.backFaceStencil.failOp);
    rhi::HashCombine(key, desc.renderState.depthStencilState.backFaceStencil.passOp);
    rhi::HashCombine(key, desc.renderState.depthStencilState.backFaceStencil.stencilFunc);
    rhi::HashCombine(key, desc.renderState.depthStencilState.frontFaceStencil.depthFailOp);
    rhi::HashCombine(key, desc.renderState.depthStencilState.frontFaceStencil.passOp);
    rhi::HashCombine(key, desc.renderState.depthStencilState.frontFaceStencil.failOp);
    rhi::HashCombine(key, desc.renderState.depthStencilState.stencilEnable);
    rhi::HashCombine(key, desc.renderState.depthStencilState.stencilReadMask);
    rhi::HashCombine(key, desc.renderState.depthStencilState.stencilRefValue);
    rhi::HashCombine(key, desc.renderState.depthStencilState.stencilWriteMask);

    rhi::HashCombine(key, desc.renderState.rasterState.antialiasedLineEnable);
    rhi::HashCombine(key, desc.renderState.rasterState.conservativeRasterEnable);
    rhi::HashCombine(key, desc.renderState.rasterState.cullMode);
    rhi::HashCombine(key, desc.renderState.rasterState.depthBias);
    rhi::HashCombine(key, desc.renderState.rasterState.depthBiasClamp);
    rhi::HashCombine(key, desc.renderState.rasterState.depthClipEnable);

    rhi::HashCombine(key, desc.renderState.blendState.alphaToCoverageEnable);
    for (size_t i = 0; i < rhi::MAX_RENDER_TARGETS; i++)
    {
        rhi::HashCombine(key, desc.renderState.blendState.targets[i].blendEnable);
        rhi::HashCombine(key, desc.renderState.blendState.targets[i].blendOp);
        rhi::HashCombine(key, desc.renderState.blendState.targets[i].blendOpAlpha);
        rhi::HashCombine(key, desc.renderState.blendState.targets[i].colorWriteMask.ToString());
        rhi::HashCombine(key, desc.renderState.blendState.targets[i].destBlend);
        rhi::HashCombine(key, desc.renderState.blendState.targets[i].destBlendAlpha);
    }

    rhi::HashCombine(key, desc.bindingLayouts.size());
    return key;
}

GraphicsPipeline* GraphicsPipeline::Create(DrawContext& context, PipelineCache& cacheData)
{
    GraphicsPipelinePtr pipeline = nullptr;
    auto service = context.GetResourceService();
    if (service->GetGraphicsPipelinePool()->Get(cacheData.key, pipeline))
    {
        return pipeline;
    }

    rhi::GraphicsPipelineDesc desc = {};
    rhi::PipelineCache pipelineCache = {};
    pipelineCache.cache = cacheData.data.data();
    pipelineCache.cacheName = cacheData.name.c_str();
    pipelineCache.cacheSize = cacheData.data.size();

    pipeline.reset(new GraphicsPipeline);
    pipeline->m_pipeline = context.GetDevice()->CreateGraphicsPipelineFromCache(pipelineCache, desc);
    service->GetGraphicsPipelinePool()->Add(cacheData.key, pipeline);
    return pipeline;
}


GraphicsPipeline* GraphicsPipeline::Create(
    DrawContext& context,
    const MaterialPtr& material,
    const PipelineInfo::DrawSettings& pipelineDrawSettings,
    const DrawObject::Settings& drawObjectDrawSettings,
    const rhi::GraphicsState& state,
    const core::TVector<rhi::IBindingLayout*> bindingLayouts)
{

    rhi::GraphicsPipelineDesc desc = {};
    SetupShader(material, desc);
    SetupBindingLayouts(bindingLayouts, desc);
    SetupVertexAttribute(context, state, desc);
    SetupRasterState(material, pipelineDrawSettings, desc);
    SetupDepth(pipelineDrawSettings, desc);
    SetupStencil(pipelineDrawSettings, drawObjectDrawSettings.stencilValue, desc);

    uint64_t key = CreatePipelineKey(desc);
    GraphicsPipelinePtr pipeline = nullptr;
    auto service = context.GetResourceService();
    if (service->GetGraphicsPipelinePool()->Get(key, pipeline))
    {
        return pipeline;
    }

    pipeline.reset(new GraphicsPipeline());
    pipeline->m_key = key;
    pipeline->m_name = material->GetName();
    pipeline->m_pipeline = context.GetDevice()->CreateGraphicsPipeline(desc, state.framebuffer);
    service->GetGraphicsPipelinePool()->Add(key, pipeline);
    return pipeline;

}

GraphicsPipeline* GraphicsPipeline::Create(
    DrawContext& context,
    const PipelineInfo::DrawSettings& pipelineDrawSettings,
    const PipelineInfo::PostProcessSettings& postProcessSettings,
    const rhi::GraphicsState& state,
    const rhi::BindingLayoutPtr& bindingLayout
)
{
    rhi::GraphicsPipelineDesc desc = {};
    postProcessSettings.shader->GetVertexShader(desc.vs);
    postProcessSettings.shader->GetPixelShader(desc.ps);
    desc.bindingLayouts.push_back(bindingLayout);
    SetupDepth(pipelineDrawSettings, desc);
    SetupStencil(pipelineDrawSettings, postProcessSettings.stencilValue, desc);

    uint64_t key = CreatePipelineKey(desc);
    GraphicsPipelinePtr pipeline = nullptr;
    auto service = context.GetResourceService();
    if (service->GetGraphicsPipelinePool()->Get(key, pipeline))
    {
        return pipeline;
    }

    pipeline.reset(new GraphicsPipeline());
    pipeline->m_key = key;
    pipeline->m_name = postProcessSettings.shader->GetName();
    pipeline->m_pipeline = context.GetDevice()->CreateGraphicsPipeline(desc, state.framebuffer);
    service->GetGraphicsPipelinePool()->Add(key, pipeline);
    return pipeline;
}

void GraphicsPipeline::SetupShader(const MaterialPtr& material, rhi::GraphicsPipelineDesc& desc)
{
    core::TVector<StaticSwitches> staticSwitces = {};
    material->GetStaticSwitches(staticSwitces);
    material->GetShader()->GetVertexShader(false, desc.vs);
    material->GetShader()->GetPixelShader(staticSwitces, material->GetDrawSettings(), desc.ps);
    material->GetShader()->GetGeometryShader(desc.gs);
    material->GetShader()->GetDomainShader(desc.ds);
    material->GetShader()->GetHullShader(desc.hs);
}

void GraphicsPipeline::SetupBindingLayouts(const core::TVector<rhi::IBindingLayout*> bindingLayouts, rhi::GraphicsPipelineDesc& desc)
{
    size_t size = math::TMin(bindingLayouts.size(), (size_t)rhi::MAX_BINDING_LAYOUTS);
    for (size_t i = 0; i < size; i++)
    {
        desc.bindingLayouts.push_back(bindingLayouts.at(i));
    }
}

void GraphicsPipeline::SetupVertexAttribute(DrawContext& context, const rhi::GraphicsState& state, rhi::GraphicsPipelineDesc& desc)
{
    core::TVector<rhi::VertexAttributeDesc> vertexAttributeDesc = {};
    vertexAttributeDesc.resize(state.vertexBuffers.size());
    for (auto& vertexBuffer : state.vertexBuffers)
    {
        rhi::VertexAttributeDesc& attribute = vertexAttributeDesc.emplace_back();
        attribute.name = vertexBuffer.shaderLayoutName;
        attribute.bufferIndex = vertexBuffer.slot;
        attribute.arraySize = 1;
        attribute.format = vertexBuffer.format;
        attribute.elementStride = vertexBuffer.stride;
    }
    desc.inputLayout = context.GetDevice()->CreateInputLayout(vertexAttributeDesc.data(), static_cast<uint32_t>(vertexAttributeDesc.size()), desc.vs);
}

void GraphicsPipeline::SetupRasterState(const MaterialPtr& material, const PipelineInfo::DrawSettings& drawSettings, rhi::GraphicsPipelineDesc& desc)
{
    desc.renderState.rasterState.frontCounterClockwise = RasterizeUtility::GetFrontCounterClockWise();
    if (drawSettings.cullMode == RasterCullMode::FromObject)
    {
        desc.renderState.rasterState.cullMode = material->GetDrawSettings().twoSide ? rhi::RasterCullMode::None : rhi::RasterCullMode::Back;
    }
    else
    {
        desc.renderState.rasterState.cullMode = static_cast<rhi::RasterCullMode>(drawSettings.cullMode);
    }
    desc.renderState.blendState.alphaToCoverageEnable = material->GetDrawSettings().alphaToCoverage;
}

void GraphicsPipeline::SetupDepth(const PipelineInfo::DrawSettings& drawSettings, rhi::GraphicsPipelineDesc& desc)
{
    desc.renderState.depthStencilState.depthWriteEnable = drawSettings.depthWriteEnable;
    desc.renderState.depthStencilState.depthFunc = static_cast<rhi::ComparisonFunc>(drawSettings.depthFunc);
}

void GraphicsPipeline::SetupStencil(const PipelineInfo::DrawSettings& pipelineDrawSettings, uint8_t stencilValue, rhi::GraphicsPipelineDesc& desc)
{
    desc.renderState.depthStencilState.stencilEnable = pipelineDrawSettings.stencilEnable;
    desc.renderState.depthStencilState.stencilReadMask = pipelineDrawSettings.stencilReadMask;
    desc.renderState.depthStencilState.stencilWriteMask = stencilValue;
    desc.renderState.depthStencilState.stencilRefValue = stencilValue;

    rhi::DepthStencilState::StencilOpDesc frontDesc = {};
    frontDesc.passOp = static_cast<rhi::StencilOp>(pipelineDrawSettings.frontFaceStencil);
    desc.renderState.depthStencilState.frontFaceStencil = frontDesc;

    rhi::DepthStencilState::StencilOpDesc backDesc = {};
    backDesc.passOp = static_cast<rhi::StencilOp>(pipelineDrawSettings.backFaceStencil);
    desc.renderState.depthStencilState.backFaceStencil = backDesc;
}

rhi::GraphicsPipelinePtr& GraphicsPipeline::GetPipeline()
{
    return m_pipeline;
}

PipelineCache GraphicsPipeline::GetCache() const
{
    PipelineCache cache;
    m_pipeline->GetCache(cache.data);
    cache.name = m_name;
    cache.key = m_key;
    return cache;
}


}
}
