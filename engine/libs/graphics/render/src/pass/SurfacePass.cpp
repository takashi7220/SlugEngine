#include "render/pass/SurfacePass.hpp"
#include "render/utility/Utility.hpp"
#include "render/context/BindingCache.hpp"
#include "render/context/DrawContext.hpp"
#include "render/pipeline/GraphicsPipeline.hpp"
#include "render/utility/ScopedMarker.hpp"

namespace slug::render
{

void SurfacePass::Setup(const SetupParam& setup)
{

}

void SurfacePass::Prepare(const PipelineInfo& pipelineInfo)
{
    if (pipelineInfo.surfaceSettings.drawObjectCuller == nullptr
        || pipelineInfo.surfaceSettings.primitiveDrawObject== nullptr)
    {
        SLUG_ASSERT_MSG(false, "DrawObjects or DrawObjectCuller is nullptr");
        return;
    }

    CullingSettings settings = {};
    pipelineInfo.surfaceSettings.drawObjectCuller->Culling(pipelineInfo.surfaceSettings.primitiveDrawObject->drawObjects, settings);
}

void SurfacePass::Draw(DrawContext& context, const PipelineInfo& pipelineInfo)
{
    bool enable = pipelineInfo.commandList != nullptr && pipelineInfo.framebuffer != nullptr && pipelineInfo.surfaceSettings.drawObjectCuller != nullptr;

    if (!enable)
    {
        return;
    }

    bool lastTwoSide = false;
    MaterialPtr lastMaterial = nullptr;
    GeometroyAttributePtr lastGeometry = nullptr;

    rhi::GraphicsState graphicsState;
    graphicsState.framebuffer = pipelineInfo.framebuffer->GetFramebuffer(context, pipelineInfo.view);
    graphicsState.viewport = pipelineInfo.view->GetViewportState();
    graphicsState.shadingRateState = pipelineInfo.view->GetVariableRateShadingState();

    rhi::DrawArguments currentDraw;
    currentDraw.instanceCount = 0;

    bool needUpdateState = true;


#if DEBUG_MODE
    ScopedMarker pipelineScopeMarker(pipelineInfo.commandList, pipelineInfo.pipelineName);
#endif

    // 描画アイテム毎に描画
    while (const DrawObjectPtr item = pipelineInfo.surfaceSettings.drawObjectCuller->GetNextDrawObject())
    {

        bool enableDrawObject = (item != nullptr && item->GetMesh() != nullptr && item->GetMaterial() != nullptr);
        if (!enableDrawObject)
        {
            continue;
        }

        MaterialPtr newMaterial = item->GetMaterial();
        const DrawSettings& newDrawSettings = newMaterial->GetDrawSettings();

        GeometroyAttributePtr newGeometry = item->GetMesh()->GetGeometry();

        // 1つ前と違うマテリアル、バッファか確認
        bool isNewMaterial = (newMaterial != lastMaterial) || (newDrawSettings.twoSide != lastTwoSide);
        bool isNewBuffer = newGeometry != lastGeometry;

        // 新しい場合は描画
        if (isNewMaterial || isNewBuffer)
        {
#if DEBUG_MODE
            ScopedMarker itemScopeMarker(pipelineInfo.commandList, item->GetName());
#endif
            FlushDraw(pipelineInfo.commandList, graphicsState, currentDraw);
        }

        // バッファが新しい場合はセットアップ
        if (isNewBuffer)
        {
            SetupGeometry(newGeometry, graphicsState);
            lastGeometry = newGeometry;
            needUpdateState = true;
        }

        // マテリアルが新しい場合はセットアップ
        if (newMaterial)
        {
            SetupGraphicsPipeline(context, pipelineInfo.drawSettings,  *pipelineInfo.view, *item, item->GetMaterial(), graphicsState);
            lastMaterial = newMaterial;
            needUpdateState = true;
        }

        // ステートの更新が必要な場合は更新
        if (needUpdateState)
        {
            pipelineInfo.commandList->SetGraphicsState(graphicsState);
            needUpdateState = false;
        }

        rhi::DrawArguments args;
        args.vertexCount = static_cast<uint32_t>(item->GetMesh()->GetVertexCount());
        args.instanceCount = 1;
        args.startVertexLocation = 0;
        args.startIndexLocation = 0;
        args.startInstanceLocation = 0;

        if (currentDraw.instanceCount > 0 &&
            currentDraw.startIndexLocation == args.startIndexLocation &&
            currentDraw.startInstanceLocation + currentDraw.instanceCount == args.startInstanceLocation)
        {
            currentDraw.instanceCount += 1;
        }
        else
        {
#if DEBUG_MODE
            ScopedMarker itemScopeMarker(pipelineInfo.commandList, item->GetName());
#endif
            FlushDraw(pipelineInfo.commandList, graphicsState, currentDraw);
            currentDraw = args;
        }
    }

    FlushDraw(pipelineInfo.commandList, graphicsState, currentDraw);
}

void SurfacePass::GetBinding(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache)
{

}

void SurfacePass::FlushDraw(rhi::ICommandList* commandList, rhi::GraphicsState& state, rhi::DrawArguments& drawArg)
{
    if (drawArg.instanceCount == 0)
    {
        return;
    }

    commandList->DrawIndexed(drawArg);
    drawArg.instanceCount = 0;
}

void SurfacePass::SetupGraphicsPipeline(
        DrawContext& context,
        const PipelineInfo::DrawSettings& pipelineDrawSettings,
        const View& view,
        const DrawObject& drawObject,
        const MaterialPtr& material,
        rhi::GraphicsState& state)
{

    core::TVector<rhi::IBindingLayout*> bindingLayouts = {};
    bindingLayouts.push_back(view.GetBindingLayout());
    bindingLayouts.push_back(drawObject.GetBindingLayout());
    bindingLayouts.push_back(material->GetBindingLayout());

    auto pipeline = GraphicsPipeline::Create(context, material, pipelineDrawSettings, drawObject.GetSettings(), state, bindingLayouts);
    state.pipeline = pipeline->GetPipeline();
    state.bindings = {view.GetBindingSet(), drawObject.GetBindingSet(), material->GetBindingSet()};
}

void SurfacePass::SetupGeometry(const GeometroyAttributePtr& geometry, rhi::GraphicsState& state)
{
    state.vertexBuffers.resize(0);
    for (auto& attributeItem : geometry->vertexAttribute.attributes)
    {
        auto& vertexBufferBinding = state.vertexBuffers.emplace_back();
        vertexBufferBinding.buffer = geometry->vertexAttribute.buffer;
        vertexBufferBinding.slot = SlotUtility::GetSlotIndex(attributeItem.attributeName.c_str());
        vertexBufferBinding.shaderLayoutName = attributeItem.attributeName.c_str();
        vertexBufferBinding.format = attributeItem.formatType;
        vertexBufferBinding.offset = attributeItem.bufferRange.byteOffset;
    }
    state.indexBuffer = rhi::IndexBufferBinding {geometry->indexAttribute.buffer, geometry->indexAttribute.formatType, 0};
}

}
