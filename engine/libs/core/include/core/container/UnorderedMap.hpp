#pragma once

#if defined(SLUG_USE_STL)
#include <unordered_map>
#endif

#include "core/TypeConcept.hpp"
#include "core/memory/Allocator.hpp"
#include "core/container/Pair.hpp"
namespace slug
{
    namespace core
    {
#if defined(SLUG_USE_STL)
        template<typename Key>
        using THash = std::hash<Key>;

        template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>, typename Allocator = TAllocator<TPair<const Key, Value>>>
        using TUnorderedMap = std::unordered_map<Key, Value, Hash, Equal, Allocator>;
#else
#endif

    }
}