#pragma once

#if defined(SLUG_USE_STL)
#include <vector>
#endif

#include "core/container/Span.hpp"

namespace slug
{
namespace core
{

#if defined(SLUG_USE_STL)
template<typename T, typename Alloc = TAllocator<T>>
using TVector = std::vector<T, Alloc>;
#else
#endif

template<typename T>
struct is_vector : std::false_type
{
};
template<typename T, typename Alloc>
struct is_vector<core::TVector<T, Alloc>> : std::true_type
{
};
template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;


}
}
