#pragma once

#if defined(SLUG_USE_STL)
#include <atomic>
#endif

namespace slug::core
{
#if defined(SLUG_USE_STL)
    template<typename T>
    using TAtomic = std::atomic<T>;

    inline constexpr std::memory_order MemoryOrderRelaxed = std::memory_order::relaxed;
    inline constexpr std::memory_order MemoryOrderConsume = std::memory_order::consume;
    inline constexpr std::memory_order MemoryOrderAcquire = std::memory_order::acquire;
    inline constexpr std::memory_order MemoryOrderRelease = std::memory_order::release;
    inline constexpr std::memory_order MemoryOrderAcqRel = std::memory_order::acq_rel;
    inline constexpr std::memory_order MemoryOrderSeqCst = std::memory_order::seq_cst;
#else
#endif
}
