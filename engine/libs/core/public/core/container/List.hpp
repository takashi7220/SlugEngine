#pragma once

#if defined(SLUG_USE_STL)
#include <list>
#endif

#include "core/container/Span.hpp"

namespace slug::core
{
#if defined(SLUG_USE_STL)
template<typename T>
using TList = std::list<T, TAllocator<T>>;
#else
#endif
}
