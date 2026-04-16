#include "engine_core/asset/AssetBase.hpp"
#include "engine_core/asset/AssetResolver.hpp"
namespace slug::engine_core
{

AssetBase::AssetBase(const AssetHeader& header)
    : m_assetHeader(header)
    , m_id(AssetType(header.filePath), AssetPath(header.assetType))
{
}

const AssetType& AssetBase::GetAssetType() const
{
    return m_id.type;
}

const AssetID& AssetBase::GetID() const
{
    return m_id;
}

const AssetPath& AssetBase::GetPath() const
{
    return m_id.path;
}

}
