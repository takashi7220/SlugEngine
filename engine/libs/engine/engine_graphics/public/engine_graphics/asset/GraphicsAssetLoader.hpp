#pragma once

#include "engine_core/asset/AssetLoaderBase.hpp"
#include "render_core/service/RenderService.hpp"

namespace slug::engine_graphics
{

class GraphicsAssetLoader : public engine_core::AssetLoaderBase
{
public:
    GraphicsAssetLoader(render_core::RenderService& service);
    virtual engine_core::AssetBasePtr LoadInternal(const LoadParam& param) override;

private:
    render_core::DrawContext& m_context;
};

class GraphicsAssetBase;
using GetGraphicsAssetTypePtr = engine_core::AssetType(*)();
using GenerateGraphicsAssetPtr = core::TReferencePtr<GraphicsAssetBase>(*)(const engine_core::AssetHeader&, render_core::DrawContext&);

class GraphicsAssetUtility
{
public:
    static void RegisterGenerator(GetGraphicsAssetTypePtr getFunc, GenerateGraphicsAssetPtr createFunc);
    static bool HasGenerator(const engine_core::AssetHeader& header);
    static core::TReferencePtr<GraphicsAssetBase> Generate(const engine_core::AssetHeader& header, render_core::RenderService& service);
};
}
