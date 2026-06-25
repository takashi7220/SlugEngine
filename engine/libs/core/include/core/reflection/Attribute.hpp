#pragma once

#include "core/container/String.hpp"

namespace slug::core
{

using TypeId = uint64_t;

SLUG_PROPERTY()
enum class PropertyEditType : uint8_t
{
    Editable,
    Readonly
};

struct PropertyDescriptor
{
    PropertyEditType type;
    core::FixedString<64> category;
    core::FixedString<64> displayName;
    core::FixedString<256> tooltip;
    core::FixedString<64> groupName;

    PropertyDescriptor()
        : type(PropertyEditType::Editable)
        , category("")
        , displayName("")
        , tooltip("")
        , groupName("")
    {
    }

    PropertyDescriptor(PropertyEditType _type, core::StringView _category, core::StringView _displayName, core::StringView _tooltip, core::StringView _groupName)
        : type(_type)
        , category(_category)
        , displayName(_displayName)
        , tooltip(_tooltip)
        , groupName(_groupName)
    {
    }
};
}
