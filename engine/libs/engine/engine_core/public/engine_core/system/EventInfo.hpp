#pragma once

#include "core/uuid/UUID.hpp"
#include "engine_core/component/ComponentInfo.hpp"

namespace slug::engine_core
{

enum class EventType
{
    Create,
    Destroy,
    OtherAdd,
    OtherRemove,
    OnSelected,
};

struct EventInfo
{
    EventType type;
    ComponentID componentId;
};
}
