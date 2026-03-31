#pragma once

#include "engine_core/asset/AssetBase.hpp"
#include "core/container/Vector.hpp"



namespace slug
{
namespace engine_core
{

class AssetResolver
{
public:
    AssetResolver(const core::TVector<AssetBasePtr>& dependencyAssets, const core::TVector<AssetBasePtr>& defaultAssets)
        : m_dependencyAssets(dependencyAssets)
        , m_defaultAssets(defaultAssets)
    {
    }

    bool HasAsset(const AssetID& id) const
    {
        for (auto& asset : m_dependencyAssets)
        {
            if (asset->GetID() == id)
            {
                return true;
            }
        }
        return false;
    }

    template<DerivedFromAssetBase T> core::TReferencePtr<T> GetAsset(const AssetID& id) const
    {
        for (auto& asset : m_dependencyAssets)
        {
            if (asset->GetID() == id)
            {
                return std::dynamic_pointer_cast<T>(asset);
            }
        }

        for (auto& asset : m_defaultAssets)
        {
            if (asset->GetAssetType() == id.type)
            {
                return std::dynamic_pointer_cast<T>(asset);
            }
        }

        SLUG_ASSERT_MSG(false, "Cant Find Asset. return nullptr");
        return nullptr;
    }

private:
    core::TVector<AssetBasePtr> m_dependencyAssets;
    core::TVector<AssetBasePtr> m_defaultAssets;
};
}
}
