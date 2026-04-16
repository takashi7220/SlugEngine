#pragma once

#include "core/container/String.hpp"

namespace slug::core
{

SLUG_PROPERTY()
enum class PropertyEditType : uint8_t
{
    EditAnywhere,
    EditDefaultsOnly,
    EditInstanceOnly,
    VisibileAnywhere,
    VisibleDefaultsOnly,
    VisibleInstanceOnly
};

struct PropertyDescriptor
{
    PropertyEditType type;
    core::FixedString<64> category;
    core::FixedString<64> displayName;
    core::FixedString<256> tooltip;
    core::FixedString<64> groupName;

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
