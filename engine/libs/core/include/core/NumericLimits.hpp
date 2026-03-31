#pragma once

#if defined(SLUG_USE_STL)
#include <limits>
#endif

#include "math/TypeConcept.hpp"

namespace slug
{
namespace core
{
#if defined(SLUG_USE_STL)
    template<math::NumericType T>
    using TNumericLimits = std::numeric_limits<T>;
#else
#endif

}
}
