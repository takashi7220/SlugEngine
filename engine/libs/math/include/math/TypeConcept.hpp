#pragma once

#include <cstdint>
#include <type_traits>
#include <concepts>

namespace slug::math
{

template<typename T>
concept NumericType = std::is_same_v<T, int8_t>    ||
                      std::is_same_v<T, uint8_t>   ||
                      std::is_same_v<T, int16_t>   ||
                      std::is_same_v<T, uint16_t>  ||
                      std::is_same_v<T, int32_t>   ||
                      std::is_same_v<T, uint32_t>  ||
                      std::is_same_v<T, int64_t>   ||
                      std::is_same_v<T, uint64_t>  ||
                      std::is_same_v<T, float> ||
                      std::is_same_v<T, double> ||
                      std::is_same_v<T, long double> ||
                      std::is_same_v<T, long>;

template<typename T>
concept FloatingType = std::is_same_v<T, float> ||
                       std::is_same_v<T, double> ||
                       std::is_same_v<T, long double>;
}
