#pragma once

#include "engine_core/asset/AssetHeader.hpp"
#include "engine_core/asset/AssetUtility.hpp"
#include "core/object/SObject.hpp"

#include "core/io/Path.hpp"

#define SLUG_ASSET_REGISTER(type)                                                                         \
namespace                                                                                                 \
{                                                                                                         \
    struct AutoRegisterGenerator_##type                                                                     \
    {                                                                                                     \
        AutoRegisterGenerator_##type()                                                                      \
        {                                                                                                 \
            slug::engine_core::AssetUtility::RegisterGenerator(&type::GetStaticAssetType, &type::Create);   \
        }                                                                                                 \
    } autoRegisterGenerator_##type;                                                                         \
}                                                                                                         \

#define SLUG_ASSET_MEMBER_DEFINE(type)                                                          \
static engine_core::AssetType GetStaticAssetType()                                              \
{                                                                                               \
    return engine_core::AssetType(#type);                                                       \
}                                                                                               \
                                                                                                \
static core::TReferencePtr<engine_core::AssetBase> Create(const engine_core::AssetHeader& header)  \
{                                                                                               \
    core::TReferencePtr<type> asset(new type(header));                                             \
    return std::dynamic_pointer_cast<engine_core::AssetBase>(asset);                            \
}                                                                                               \

namespace slug
{
namespace engine_core
{

class AssetResolver;

class AssetBase : public core::SObject
{
public:
    AssetBase(const AssetHeader& header);

    const AssetType& GetAssetType() const;
    const AssetID& GetID() const;
    const AssetPath& GetPath() const;

    virtual bool Load(void* data, size_t size) = 0;
    virtual bool Resolve(const AssetResolver& resolver) = 0;
    virtual bool Reload(void* data, size_t size) = 0;
    virtual void Destroy() = 0;

protected:
    AssetHeader m_assetHeader;
    AssetID m_id;
};

using AssetBasePtr = core::TReferencePtr<AssetBase>;

template<typename T>
concept DerivedFromAssetBase = std::is_base_of_v<AssetBase, T>;

}
}
