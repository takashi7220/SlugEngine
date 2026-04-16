#include "engine_graphics/asset/GraphicsAssetLoader.hpp"
#include "engine_graphics/asset/GraphicsAssetBase.hpp"

namespace slug::engine_graphics
{

GraphicsAssetLoader::GraphicsAssetLoader(render::DrawContext& context)
    : m_context(context)
{

}

engine_core::AssetBasePtr GraphicsAssetLoader::LoadInternal(const LoadParam& param)
{
    auto asset = GraphicsAssetUtility::Generate(param.header, m_context);
    asset->Load(param.data, param.size);
    return asset;
}

static core::TUnorderedMap<engine_core::AssetType, GenerateGraphicsAssetPtr, engine_core::AssetTypeKeyHash, engine_core::AssetTypeKeyCompare> GRAPHICS_ASSET_GENERATOR_TABLE = {};

void GraphicsAssetUtility::RegisterGenerator(GetGraphicsAssetTypePtr getFunc, GenerateGraphicsAssetPtr createFunc)
{
    engine_core::AssetType type = getFunc();
    if (GRAPHICS_ASSET_GENERATOR_TABLE.find(type) == GRAPHICS_ASSET_GENERATOR_TABLE.end())
    {
        GRAPHICS_ASSET_GENERATOR_TABLE[type] = createFunc;
    }
}

bool GraphicsAssetUtility::HasGenerator(const engine_core::AssetHeader& header)
{
    engine_core::AssetType type(header.assetType);
    if (GRAPHICS_ASSET_GENERATOR_TABLE.find(type) != GRAPHICS_ASSET_GENERATOR_TABLE.end())
    {
        return true;
    }
    return false;
}

core::TReferencePtr<GraphicsAssetBase> GraphicsAssetUtility::Generate(const engine_core::AssetHeader& header, render::DrawContext& context)
{
    engine_core::AssetType type(header.assetType);
    if (GRAPHICS_ASSET_GENERATOR_TABLE.find(type) != GRAPHICS_ASSET_GENERATOR_TABLE.end())
    {
        return GRAPHICS_ASSET_GENERATOR_TABLE[type](header, context);
    }

    SLUG_ASSERT_MSG(false, core::StringUtility::Sprintfs("Cant Find Asset Generator [%s]", type.name.c_str()).c_str());
    return nullptr;
}

}
