#pragma once

#include "core/service/ServiceProvider.hpp"
#include "core/task/TaskSystem.hpp"
#include "resource/asset/IAsset.hpp"

namespace slug::resource
{

class AssetService : public core::IService
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(AssetService, core::IService)
    SLUG_SERVICE_REGISTER_DEPENDENTS(AssetService, core::TaskSystem)

    AssetService(core::TaskSystem* taskSystem);

    static AssetId MakeAssetId(core::StringView typeName, core::StringView assetName);
    static AssetId MakeAssetId(AssetTypeId typeId, core::StringView assetName);

    IAsset* GetAsset(AssetId assetId)
    {
        auto it = m_assetIdToIndexMap.find(assetId);
        if (it != m_assetIdToIndexMap.end())
        {
            return m_assets[it->second];
        }
        return nullptr;
    }

    template<typename AssetType>
    AssetType* GetAsset(AssetId assetId)
    {
        IAsset* asset = GetAsset(assetId);
        return core::SObjectUtility::CheckedCast<AssetType>(asset);
    }

    template<typename AssetType>
    AssetType* GetAsset(core::StringView assetName)
    {
        AssetId assetId = MakeAssetId(AssetType::GetStaticTypeId(), assetName);
        IAsset* asset = GetAsset(assetId);
        return core::SObjectUtility::CheckedCast<AssetType>(asset);
    }

private:
    core::TReferencePtr<core::TaskSystem> m_taskSystem;
    core::TUnorderedMap<AssetId, uint64_t> m_assetIdToIndexMap;
    core::TVector<core::TReferencePtr<IAsset>> m_assets;
};
}
