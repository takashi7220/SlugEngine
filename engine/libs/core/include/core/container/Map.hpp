#pragma once

#if defined(SLUG_USE_STL)
#include <map>
#endif

#include "core/TypeConcept.hpp"
#include "core/memory/Allocator.hpp"
#include "core/container/Pair.hpp"

namespace slug
{
namespace core
{
template<typename T>
struct Tless {
    SLUG_NODISCARD bool operator()(const T& left, const T& right) const
    {
        return left < right;
    }
};
#if defined(SLUG_USE_STL)
    template<typename Key, typename Value>
    using TMap = std::map<Key, Value, Tless<Key>, TAllocator<TPair<Key, Value>>>;
#else
#endif
}
}
