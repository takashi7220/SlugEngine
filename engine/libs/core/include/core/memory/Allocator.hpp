#pragma once
#include "core/memory/MemoryUtilities.hpp"
#include "core/debug/Exception.hpp"
#include "core/NumericLimits.hpp"
namespace slug::core
{

template<typename T>
class TAllocator
{
public:
    using value_type = T;

    TAllocator() = default;
    template<typename U>
    constexpr TAllocator(const TAllocator<U>&) noexcept {}

    T* allocate(size_t size)
    {
        if (!CheckAllocatable(size))
        {
            SLUG_THROW_EXCEPTION("Error. The allocated size is too large.");
        }
        return static_cast<T*>(MemoryUtilities::Allocate(size * sizeof(T), static_cast<size_t>(std::align_val_t(alignof(T)))));
    }

    T* allocate(size_t size, size_t align)
    {
        if (!CheckAllocatable(size))
        {
            SLUG_THROW_EXCEPTION("Error. The allocated size is too large.");
        }
        return static_cast<T*>(MemoryUtilities::Allocate(size * sizeof(T), align));
    }

    void deallocate(T* p, size_t size) noexcept
    {
        MemoryUtilities::Deallocate((void*)p);
    }

    void deallocate(T* p, size_t align, size_t size) noexcept
    {
        MemoryUtilities::Deallocate((void*)p, align);
    }

    bool CheckAllocatable(size_t size)
    {
        return size <= (ULLONG_MAX / sizeof(T));
    }
};
}
