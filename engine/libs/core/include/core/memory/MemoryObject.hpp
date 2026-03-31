#pragma once

#include "core/memory/MemoryUtilities.hpp"

namespace slug
{
namespace core
{

class MemoryObject
{
public:
    void* operator new(size_t size) noexcept;
    void* operator new[](size_t size) noexcept;

    void* operator new  (size_t size, std::align_val_t align) noexcept;
    void* operator new[](size_t size, std::align_val_t align) noexcept;

    void operator delete(void* p) noexcept;
    void operator delete[](void* p) noexcept;

    void operator delete  (void* p, std::align_val_t align) noexcept;
    void operator delete[](void* p, std::align_val_t align) noexcept;

    virtual ~MemoryObject() = default;

protected:
    static void* Allocate(size_t size);
    static void* Allocate(size_t size, std::align_val_t align);
    static void Deallocate(void* p);
    static void Deallocate(void* p, size_t size);
    static void Deallocate(void* p, std::align_val_t align);
    static void Deallocate(void* p, size_t size, std::align_val_t align);
};
}
}
