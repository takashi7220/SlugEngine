#pragma once

#if defined(SLUG_USE_STL)
#include <unordered_set>
#endif

#include "core/TypeConcept.hpp"
#include "core/memory/Allocator.hpp"
#include "core/container/UnorderedMap.hpp"

namespace slug::core
{
#if defined(SLUG_USE_STL)

template<typename Key, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>, typename Allocator = TAllocator<Key>>
using TUnorderedSet = std::unordered_set<Key, Hash, Equal, Allocator>;
#else
#endif
}
