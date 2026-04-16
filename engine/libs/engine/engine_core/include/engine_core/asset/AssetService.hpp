#pragma once

#include "engine_core/asset/AssetPool.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::engine_core
{

class AssetService : public core::IService
{
public:
    SLUG_SERVICE_REGISTER(AssetService)

    AssetService()
    {
    }

    void Initialize(AssetPoolPtr pool);
    void Terminate();
    void AddAsset(AssetBasePtr asset);
    void DeleteAsset(const AssetID& id);
    AssetBasePtr GetAsset(const AssetID& id) const;
    template<DerivedFromAssetBase T> core::TReferencePtr<T> GetAsset(const AssetID& id) const;
private:
    AssetPoolPtr m_pool;
};

}
