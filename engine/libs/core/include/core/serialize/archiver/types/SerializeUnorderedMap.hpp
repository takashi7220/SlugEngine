#pragma once

#include "core/TypeConcept.hpp"
#include "core/container/UnorderedMap.hpp"

namespace slug::core
{

template<typename Archive, typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
void Serialize(Archive& ar, const core::TUnorderedMap<Key, Value, Hash, Equal, Allocator>& value)
{
    ar.BeginArray(value.size());
    for (const auto& [k, v] : value)
    {
        ar.BeginArrayElement();
        ar.Value(k);
        ar.Value(v);
        ar.EndArrayElement();
    }
    ar.EndArray();
}

template<typename Archive, typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
void Deserialize(Archive& ar, core::TUnorderedMap<Key, Value, Hash, Equal, Allocator>& value)
{
    size_t size = ar.BeginArray();
    value.clear();
    value.reserve(size);
    for (size_t i = 0; i < size; i++)
    {
        ar.BeginArrayElement();
        Key k{};
        Value v{};
        ar.Value(k);
        ar.Value(v);
        value.emplace(std::move(k), std::move(v));
        ar.EndArrayElement();
    }
    ar.EndArray();
}

}
