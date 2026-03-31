#pragma once

#if defined(SLUG_USE_STL)
#include <deque>
#endif

#include "core/TypeConcept.hpp"
#include "core/memory/Allocator.hpp"

namespace slug
{
namespace core
{
#if defined(SLUG_USE_STL)
    template<typename T>
    using TDeque = std::deque<T, TAllocator<T>>;
#else
#endif

}
}
