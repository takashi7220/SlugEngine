#pragma once

#include "core/uuid/UUID.hpp"

namespace slug::engine_core
{
struct ComponentID
{
    core::UUID uuid;

    bool operator==(const ComponentID& id)
    {
        return id.uuid == uuid;
    }
};

struct ComponentType
{
    core::FixedString<32> name;

    ComponentType()
        : name()
    {
    }

    ComponentType(const core::StringView name)
        : name(name)
    {
    }

    bool operator==(const ComponentType& type)
    {
        return name == type.name;
    }
};

struct ComponentInfo
{
    ComponentID id;
    ComponentType type;
    uint64_t entityId;

    bool operator==(const ComponentInfo& info)
    {
        return id == info.id && type == info.type && entityId == info.entityId;
    }
};

}
