#pragma once

#include "core/serialize/archiver/types/SerializePrimitive.hpp"
#include "core/serialize/archiver/types/SerializeVector.hpp"
#include "core/serialize/archiver/types/SerializeString.hpp"

#include "core/serialize/archiver/types/SerializePrimitive.hpp"
#include "core/serialize/archiver/types/SerializeString.hpp"
#include "core/serialize/archiver/types/SerializeVector.hpp"
#include "core/serialize/archiver/archives/InputArchive.hpp"
#include "core/serialize/archiver/archives/OutputArchive.hpp"

namespace slug::core
{
template<typename Archive, typename T>
void DispatchSerialize(Archive& ar, const T& value)
{
    if constexpr (has_member_serialize<Archive, T>::value)
    {
        value.Serialize(ar);
    }
    else if constexpr (has_non_member_serialize<Archive, T>::value && !is_arithmetic_or_enum_v<T>)
    {
        Serialize(ar, value);
    }
    else
    {
        static_assert(
            has_member_serialize<Archive, T>::value ||
            has_non_member_serialize<Archive, T>::value ||
            is_arithmetic_or_enum_v<T> ||
            !is_string_v<T> ||
            !is_vector_v<T>,
            "Type is not serializable for output archive."
        );

        Serialize(ar, value);
    }
}

template<typename Archive, typename T>
void DispatchDeserialize(Archive& ar, T& value)
{
    if constexpr (has_member_deserialize<Archive, T>::value)
    {
        value.Deserialize(ar);
    }
    else if constexpr (has_non_member_deserialize<Archive, T>::value && !is_arithmetic_or_enum_v<T>)
    {
        Deserialize(ar, value);
    }
    else
    {
        static_assert(
            has_member_deserialize<Archive, T>::value ||
            has_non_member_deserialize<Archive, T>::value ||
            is_arithmetic_or_enum_v<T> ||
            !is_string_v<T> ||
            !is_vector_v<T>,
            "Type is not serializable for input archive."
        );

        Deserialize(ar, value);
    }
}
}
