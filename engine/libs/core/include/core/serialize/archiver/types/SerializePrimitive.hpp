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

template<IntegerType T, typename Archive>
void Serialize(Archive& ar, const T& value)
{
    ar.WriteInteger(value);
}

template<IntegerType T, typename Archive>
void Deserialize(Archive& ar, T& value)
{
    value = static_cast<T>(ar.template ReadInteger<T>());
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
