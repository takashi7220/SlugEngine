#pragma once

#include "core/utility/Hash.hpp"
#include "engine_core/asset/AssetBase.hpp"
#include "core/container/List.hpp"
#include "core/container/Vector.hpp"
#include "core/thread/Mutex.hpp"

namespace slug::engine_core
{

class AssetPool : public core::ReferenceObject
{
public:
    AssetPool() = default;

    virtual void AddAsset(AssetBasePtr asset);
    virtual bool ContainAsset(const AssetID& id);
    virtual void DeleteAsset(const AssetID& id);
    virtual core::TVector<AssetBasePtr>& GetDefaultAssets();

    virtual AssetBasePtr GetAsset(const AssetID& id) const;
    template<DerivedFromAssetBase T> core::TReferencePtr<T> GetAsset(const AssetID& id) const;

private:

    struct AssetList
    {
        core::TUnorderedMap<AssetPath, AssetBasePtr, AssetPathKeyHash, AssetPathKeyCompare> assets;
    };
    core::TUnorderedMap<AssetType, AssetList, AssetTypeKeyHash, AssetTypeKeyCompare> m_assetTable {};
    mutable core::Mutex m_mutex;
};
using AssetPoolPtr = core::TReferencePtr<AssetPool>;

}
