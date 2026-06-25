#pragma once

#include "Allocator.hpp"

namespace slug::core
{
class MemoryObject
{
public:
    void* operator new(size_t size) noexcept
    {
        return Allocate(size);
    }

    void* operator new[](size_t size) noexcept
    {
        return Allocate(size);
    }

    void* operator new  (size_t size, std::align_val_t align) noexcept
    {
        return Allocate(size);
    }

    void* operator new[](size_t size, std::align_val_t align) noexcept
    {
        return Allocate(size);
    }

    void operator delete(void* p) noexcept
    {
        Deallocate(p);
    }

    void operator delete[](void* p) noexcept
    {
        Deallocate(p);
    }

    void operator delete  (void* p, std::align_val_t align) noexcept
    {
        Deallocate(p, align);
    }

    void operator delete[](void* p, std::align_val_t align) noexcept
    {
        Deallocate(p, align);
    }

    virtual ~MemoryObject() = default;
protected:
    static void* Allocate(size_t size)
    {
        return MemoryUtility::Allocate(size);
    }

    static void* Allocate(size_t size, std::align_val_t align)
    {
        return MemoryUtility::Allocate(size, static_cast<size_t>(align));
    }

    static void Deallocate(void* p)
    {
        return MemoryUtility::Deallocate(p);
    }

    static void Deallocate(void* p, size_t size)
    {
        return MemoryUtility::Deallocate(p);
    }

    static void Deallocate(void* p, std::align_val_t align)
    {
        return MemoryUtility::Deallocate(p, static_cast<size_t>(align));
    }

    static void Deallocate(void* p, size_t size, std::align_val_t align)
    {
        return MemoryUtility::Deallocate(p, static_cast<size_t>(align));
    }
};

template<typename T, class... Args>
T* NewObject(Args&&... args)
{
    return new T(std::forward<Args>(args)...);
}
}
