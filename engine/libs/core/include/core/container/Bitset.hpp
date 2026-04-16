#pragma once

#if defined(SLUG_USE_STL)
#include <bitset>
#endif

#include "core/TypeConcept.hpp"

namespace slug::core
{
#if defined(SLUG_USE_STL)
    template<size_t Size>
    using TBitset = std::bitset<Size>;
#else
#endif

}