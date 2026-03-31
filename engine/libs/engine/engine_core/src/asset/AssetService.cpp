#include "engine_core/asset/AssetService.hpp"

namespace slug
{
namespace engine_core
{
void AssetService::Initialize(AssetPoolPtr pool)
{
    m_pool = pool;
}

void AssetService::Terminate()
{
    m_pool = nullptr;
}

void AssetService::AddAsset(AssetBasePtr asset)
{
    m_pool->AddAsset(asset);
}

void AssetService::DeleteAsset(const AssetID& id)
{
    m_pool->DeleteAsset(id);
}

AssetBasePtr AssetService::GetAsset(const AssetID& id) const
{
    return m_pool->GetAsset(id);
}

template<DerivedFromAssetBase T> core::TReferencePtr<T> AssetService::GetAsset(const AssetID& id) const
{
    return m_pool->GetAsset<T>(id);
}

}
}
