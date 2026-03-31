#pragma once

#if defined(SLUG_USE_STL)
#include <utility>
#endif

#include "core/TypeConcept.hpp"
#include "core/memory/Allocator.hpp"

namespace slug
{
namespace core
{
#if defined(SLUG_USE_STL)
    template<typename T0, typename T1>
    using TPair = std::pair<T0, T1>;

    template<typename T, typename U>
    std::pair<T, U> MakePair(T v0, U v1)
    {
        return std::make_pair(std::forward<T>(v0), std::forward<U>(v1));
    }
#else
#endif

}
}
