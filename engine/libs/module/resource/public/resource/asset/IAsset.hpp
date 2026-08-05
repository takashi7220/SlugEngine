#pragma once

#include "resource/asset/AssetHeader.hpp"
#include "core/object/SObject.hpp"
#include "core/serialize/archiver/ArchiverSerializer.hpp"

namespace slug::resource
{

class IAsset;
using IAssetPtr = core::TReferencePtr<IAsset>;

struct AssetInfo
{
    core::String name;
    core::String typeName;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("name", name);
        ar.Field("typeName", typeName);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("name", name);
        ar.Field("typeName", typeName);
        ar.EndObject();
    }
};

class IAsset : public core::SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IAsset, slug::core::SObject)
    AssetHeader header;
    AssetInfo info;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("info", info);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("info", info);
        ar.EndObject();
    }
};

}
