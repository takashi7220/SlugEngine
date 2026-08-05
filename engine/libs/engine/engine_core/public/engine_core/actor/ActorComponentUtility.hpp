#pragma once

#include "core/container/String.hpp"
#include "core/utility/Hash.hpp"
#include "core/uuid/NameID.hpp"

namespace slug::engine_core
{

struct ComponentTypeID
{
    core::NameID id;

    ComponentTypeID() = default;
    ComponentTypeID(core::StringView _id)
        : id(_id)
    {
    }

    core::StringView GetName()
    {
        return id.GetStr();
    }

    bool operator==(const ComponentTypeID& v0) const
    {
        return id == v0.id;
    }
};

struct ComponentTypeIDKeyHash
{
    size_t operator()(const ComponentTypeID& key) const
    {
        return key.id.GetHash();
    }
};

struct ComponentTypeIDKeyCompare
{
    size_t operator()(const ComponentTypeID& v0, const ComponentTypeID& v1) const
    {
        return v0 == v1;
    }
};

class ActorComponent;
using GetComponentTypeIDPtr = ComponentTypeID(*)();
using GenerateComponentPtr = ActorComponent*(*)(const ComponentTypeID&);

class ActorComponentUtility
{
public:
    static void RegisterGenerator(GetComponentTypeIDPtr getFunc, GenerateComponentPtr createFunc);
    static bool HasGenerator(const ComponentTypeID& componentTypeId);
    static ActorComponent* Generate(const ComponentTypeID& componentTypeId);
};
}
