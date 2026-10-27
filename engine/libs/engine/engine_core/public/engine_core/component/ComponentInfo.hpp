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

    bool operator==(const ComponentType& type)
    {
        return name == type.name;
    }
};

struct ComponentInfo
{
    ComponentID id;
    ComponentType type;

    bool operator==(const ComponentInfo& info)
    {
        return id == info.id && type == info.type;
    }
};

}
