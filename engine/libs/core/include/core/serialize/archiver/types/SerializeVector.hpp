#pragma once

#include "core/TypeConcept.hpp"
#include "core/container/Vector.hpp"

namespace slug::core
{

template<typename Archive, typename T>
void Serialize(Archive& ar, const core::TVector<T>& value)
{
    ar.BeginArray(value.size());
    for (const auto& element : value)
    {
        ar.BeginArrayElement();
        ar.Value(element);
        ar.EndArrayElement();
    }
    ar.EndArray();
}

template<typename Archive, typename T>
void Deserialize(Archive& ar, core::TVector<T>& value)
{
    size_t size = ar.BeginArray();
    value.resize(size);
    for (size_t i = 0; i < size; i++)
    {
        ar.BeginArrayElement();
        ar.Value(value[i]);
        ar.EndArrayElement();
    }
    ar.EndArray();
}

}
