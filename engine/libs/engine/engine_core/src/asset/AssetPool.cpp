#include "engine_core/asset/AssetPool.hpp"

namespace slug::engine_core
{
void AssetPool::AddAsset(AssetBasePtr asset)
{
    if (asset == nullptr)
    {
        return;
    }

    core::LockGuard<core::Mutex> lock(m_mutex);
    auto assetList = m_assetTable[asset->GetAssetType()];
    if (assetList.assets.find(asset->GetPath()) != assetList.assets.end())
    {
        return;
    }

    assetList.assets[asset->GetPath()] = asset;
}

void AssetPool::DeleteAsset(const AssetID& id)
{
    core::LockGuard<core::Mutex> lock(m_mutex);
    auto assetListItr = m_assetTable.find(id.type);
    if (assetListItr != m_assetTable.end())
    {
        if (assetListItr->second.assets.find(id.path) != assetListItr->second.assets.end())
        {
            assetListItr->second.assets.erase(id.path);
        }
    }
    return;
}

bool AssetPool::ContainAsset(const AssetID& id)
{
    core::LockGuard<core::Mutex> lock(m_mutex);
    auto assetListItr = m_assetTable.find(id.type);
    if (assetListItr != m_assetTable.end())
    {
        if (assetListItr->second.assets.find(id.path) != assetListItr->second.assets.end())
        {
            return true;
        }
    }
    return false;

}

AssetBasePtr AssetPool::GetAsset(const AssetID& id) const
{
    core::LockGuard<core::Mutex> lock(m_mutex);
    auto assetListItr = m_assetTable.find(id.type);
    if (assetListItr != m_assetTable.end())
    {
        auto assetItr = assetListItr->second.assets.find(id.path);
        if (assetItr != assetListItr->second.assets.end())
        {
            return assetItr->second;
        }
    }
    return nullptr;
}

template<DerivedFromAssetBase T>
core::TReferencePtr<T> AssetPool::GetAsset(const AssetID& id) const
{
    core::LockGuard<core::Mutex> lock(m_mutex);
    AssetBasePtr assetBase = this->GetAsset(id);
    return std::dynamic_pointer_cast<T>(assetBase);
}
}
