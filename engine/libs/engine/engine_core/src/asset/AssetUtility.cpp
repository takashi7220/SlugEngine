#include "engine_core/asset/AssetUtility.hpp"
#include "engine_core/asset/AssetBase.hpp"
#include "core/container/UnorderedMap.hpp"

namespace slug::engine_core
{

static core::TUnorderedMap<AssetType, GenerateAssetPtr, AssetTypeKeyHash, AssetTypeKeyCompare> ASSET_GENERATOR_TABLE = {};

void AssetUtility::RegisterGenerator(GetAssetTypePtr getFunc, GenerateAssetPtr createFunc)
{
    AssetType type = getFunc();
    if (ASSET_GENERATOR_TABLE.find(type) == ASSET_GENERATOR_TABLE.end())
    {
        ASSET_GENERATOR_TABLE[type] = createFunc;
    }
}

bool AssetUtility::HasGenerator(const AssetHeader& header)
{
    AssetType type(header.assetType);
    if (ASSET_GENERATOR_TABLE.find(type) != ASSET_GENERATOR_TABLE.end())
    {
        return true;
    }
    return false;
}

AssetBase* AssetUtility::Generate(const AssetHeader& header)
{
    AssetType type(header.assetType);
    if (ASSET_GENERATOR_TABLE.find(type) != ASSET_GENERATOR_TABLE.end())
    {
        return ASSET_GENERATOR_TABLE[type](header);
    }

    SLUG_ASSERT_MSG(false, core::StringUtility::Sprintfs("Cant Find Asset Generator [%s]", type.name.c_str()).c_str());
    return nullptr;
}

}
