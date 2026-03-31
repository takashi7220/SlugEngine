#pragma once

#if defined(SLUG_USE_STL)
#include <array>
#endif

#include "core/TypeConcept.hpp"

namespace slug 
{
namespace core 
{
#if defined(SLUG_USE_STL)
	template<typename T, size_t Size>
    using TArray = std::array<T, Size>;
#else
#endif

}
}