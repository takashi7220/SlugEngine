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
template<typename T>
using TVector = std::vector<T, TAllocator<T>>;
#else
#endif

}
}
