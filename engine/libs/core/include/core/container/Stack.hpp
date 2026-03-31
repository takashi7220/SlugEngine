#pragma once

#if defined(SLUG_USE_STL)
#include <stack>
#endif

#include "core/TypeConcept.hpp"
#include "core/allocator/TAllocator.hpp"
#include "core/container/TDeque.hpp"
namespace slug
{
namespace core
{
#if defined(SLUG_USE_STL)
    template<typename T>
    using TStack = std::stack<T, TDeque<T, TAllocator<T>>>;
#else
#endif

}
}