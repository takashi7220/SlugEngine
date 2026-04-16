#pragma once

#include "core/TypeConcept.hpp"
#include "core/container/String.hpp"

namespace slug::core
{
template<typename Archive>
void Serialize(Archive& ar, const core::String& value)
{
    ar.WriteString(value);
}

template<typename Archive>
void Deserialize(Archive& ar, core::String& value)
{
    ar.ReadString(value);
}
}
