#pragma once

#if defined(SLUG_USE_STL)
#include <atomic>
#endif

namespace slug::core
{
#if defined(SLUG_USE_STL)
    template<typename T>
    using TAtomic = std::atomic<T>;
#else
#endif
}