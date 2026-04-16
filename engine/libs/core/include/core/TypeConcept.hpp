#pragma once

#include "core/Define.hpp"
#include <type_traits>
#include <concepts>

namespace slug::core
{
template<typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

// enum判定
template<typename T>
inline constexpr bool is_arithmetic_or_enum_v = std::is_arithmetic_v<T> || std::is_enum_v<T>;

template<typename T>
concept BoolType = std::same_as<remove_cvref_t<T>, bool>;

template<typename T>
concept IntengerType = is_arithmetic_or_enum_v<T> && !std::same_as<remove_cvref_t<T>, bool> && !std::is_floating_point_v<remove_cvref_t<T>>;

template<typename T>
concept FloatType = std::is_floating_point_v<remove_cvref_t<T>>;
}
