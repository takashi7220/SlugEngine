#pragma once

#include "core/container/String.hpp"

#include "engine_core/asset/AssetHeader.hpp"
#include "core/utility/Hash.hpp"

namespace slug::engine_core
{

struct AssetType
{
    core::String name;

    AssetType(core::StringView assetTypeName)
        : name(assetTypeName)
    {
    }

    bool operator==(const AssetType& v) const
    {
        return this->name == v.name;
    }

    static AssetType Create(core::StringView typeName)
    {
        return AssetType(typeName);
    }
};

struct AssetTypeKeyHash
{
    size_t operator()(const AssetType& key) const
    {
        return core::HashUtility::ConvertStringToHash64(key.name);
    }
};

struct AssetTypeKeyCompare
{
    size_t operator()(const AssetType& v0, const AssetType& v1) const
    {
        return v0 == v1;
    }
};

struct AssetPath
{
    core::String fullpath = {};

    AssetPath(core::StringView path)
        : fullpath(path)
    {
    }

    bool operator == (const AssetPath& v) const
    {
        return this->fullpath == v.fullpath;
    }

    static AssetPath Create(core::StringView path)
    {
        return AssetPath(path);
    }
};

struct AssetPathKeyHash
{
    size_t operator()(const AssetPath& key) const
    {
        return core::HashUtility::ConvertStringToHash64(key.fullpath);
    }
};

struct AssetPathKeyCompare
{
    bool operator()(const AssetPath& v0, const AssetPath& v1) const
    {
        return v0 == v1;
    }
};


struct AssetID
{
    AssetType type;
    AssetPath path;

    AssetID()
        : path("")
        , type("")
    {
    }

    AssetID(AssetType type, AssetPath path)
        : type(type)
        , path(path)
    {
    }

    bool operator == (const AssetID& v) const
    {
        return (this->type == v.type) && (this->path == v.path);
    }
};

class AssetBase;
using GetAssetTypePtr = AssetType(*)();
using GenerateAssetPtr = AssetBase*(*)(const AssetHeader&);

class AssetUtility
{
public:
    static void RegisterGenerator(GetAssetTypePtr getFunc, GenerateAssetPtr createFunc);
    static bool HasGenerator(const AssetHeader& header);
    static AssetBase* Generate(const AssetHeader& header);
};

}
