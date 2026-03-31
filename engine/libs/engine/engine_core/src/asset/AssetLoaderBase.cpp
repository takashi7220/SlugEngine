
#include "engine_core/asset/AssetLoaderBase.hpp"
#include "engine_core/asset/AssetResolver.hpp"
#include "engine_core/asset/AssetUtility.hpp"
#include "core/thread/Future.hpp"
#include "core/pattern/Closure.hpp"

namespace slug
{
namespace engine_core
{

core::TVector<AssetBasePtr> AssetLoaderBase::Load(core::TaskSystem& taskSystem, const core::PackFile& pack, const AssetPoolPtr& assetPool)
{
    core::TVector<LoadParam> params = {};
    params.resize(pack.GetItems().size());

    taskSystem.ParallelFor(pack.GetItems().size(), [&](size_t index)
    {
        auto& item = pack.GetItems().at(index);
        LoadParam& param = params.at(index);
        core::MemoryUtilities::Memcpy(&param.header, AssetHeader::GetSize(), item.data, AssetHeader::GetSize());
        param.data = static_cast<uint8_t*>(item.data) + AssetHeader::GetSize();
        param.size = item.size - AssetHeader::GetSize();
        
    });

    return this->Load(taskSystem, params, assetPool);
}

core::TVector<AssetBasePtr> AssetLoaderBase::Load(core::TaskSystem& taskSystem, const core::TVector<LoadParam>& params, const AssetPoolPtr& assetPool)
{
    core::TVector<AssetBasePtr> dependencyAssets = {};
    dependencyAssets.reserve(params.size());

    core::TVector<LoadParam> loadItems = {};
    loadItems.reserve(params.size());

    for (const auto& param : params)
    {
        AssetID id(AssetType(param.header.assetType), AssetPath(param.header.filePath));
        if (!assetPool->ContainAsset(id))
        {
            loadItems.push_back(param);
        }
        else
        {
            dependencyAssets.push_back(assetPool->GetAsset(id));
        }
    }

    size_t dependencyAssetOffset = dependencyAssets.size();
    dependencyAssets.resize(dependencyAssetOffset + loadItems.size());

    core::TVector<AssetBasePtr> loadAssets = {};
    loadAssets.resize(loadItems.size());

    taskSystem.ParallelFor(loadItems.size(), [&](size_t index)
    {
        const auto& param = loadItems.at(index);
        auto asset = this->LoadInternal(param);
        dependencyAssets.at(dependencyAssetOffset + index) = asset;
        loadAssets.at(index) = asset;
    });

    AssetResolver resolver(dependencyAssets, assetPool->GetDefaultAssets());
    taskSystem.ParallelFor(loadAssets.size(), [&](size_t index)
    {
        auto& asset = loadAssets.at(index);
        asset->Resolve(resolver);
    });

    return dependencyAssets;
}

AssetBasePtr AssetLoaderBase::LoadInternal(const LoadParam& param)
{
    auto asset = AssetUtility::Generate(param.header);
    asset->Load(param.data, param.size);
    return asset;
}


}
}
