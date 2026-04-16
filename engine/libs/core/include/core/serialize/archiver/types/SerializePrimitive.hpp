#pragma once

#include "core/TypeConcept.hpp"

namespace slug::core
{
template<BoolType T, typename Archive>
void Serialize(Archive& ar, const T& value)
{
    ar.WriteBool(static_cast<bool>(value));
}

template<BoolType T, typename Archive>
void Deserialize(Archive& ar, T& value)
{
    value = static_cast<T>(ar.ReadBool());
}

template<IntengerType T, typename Archive>
void Serialize(Archive& ar, const T& value)
{
    ar.WriteIntenger(value);
}

template<IntengerType T, typename Archive>
void Deserialize(Archive& ar, T& value)
{
    value = static_cast<T>(ar.template ReadIntenger<T>());
}

template<FloatType T, typename Archive>
void Serialize(Archive& ar, const T& value)
{
    ar.WriteFloating(value);
}

template<FloatType T, typename Archive>
void Deserialize(Archive& ar, T& value)
{
    value = static_cast<T>(ar.template ReadFloating<T>());
}

}
