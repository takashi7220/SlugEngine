#pragma once

#include "core/TypeConcept.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"

namespace slug::core
{

// Serializeメンバー関数判定
template<typename Archive, typename T, typename = void>
struct has_member_serialize : std::false_type
{
};
template<typename Archive, typename T>
struct has_member_serialize<Archive, T, std::void_t<decltype(std::declval<const T&>().Serialize(std::declval<Archive&>()))>> : std::true_type
{
};
template<typename Archive, typename T, typename = void>
struct has_non_member_serialize : std::false_type
{
};
template<typename Archive, typename T>
struct has_non_member_serialize<Archive, T, std::void_t<decltype(Serialize(std::declval<Archive&>(), std::declval<const T&>()))>> : std::true_type
{
};

// Deserializeメンバー関数判定
template<typename Archive, typename T, typename = void>
struct has_member_deserialize : std::false_type
{
};
template<typename Archive, typename T>
struct has_member_deserialize<Archive, T, std::void_t<decltype(std::declval<T&>().Deserialize(std::declval<Archive&>()))>> : std::true_type
{
};
template<typename Archive, typename T, typename = void>
struct has_non_member_deserialize : std::false_type
{
};
template<typename Archive, typename T>
struct has_non_member_deserialize<Archive, T, std::void_t<decltype(Deserialize(std::declval<Archive&>(), std::declval<T&>()))>> : std::true_type
{
};

}
