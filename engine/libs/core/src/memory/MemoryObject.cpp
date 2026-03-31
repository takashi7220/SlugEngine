
#include "core/memory/MemoryObject.hpp"

namespace slug
{
namespace core
{
void MemoryObject::operator delete(void* p) noexcept
{
    Deallocate(p);
}

void MemoryObject::operator delete[](void* p) noexcept
{
    Deallocate(p);
}

void* MemoryObject::operator new(size_t size) noexcept
{
    return Allocate(size);
}

void* MemoryObject::operator new[](size_t size) noexcept
{
    return Allocate(size);
}

void MemoryObject::operator delete  (void* p, std::align_val_t align) noexcept
{
    Deallocate(p, align);
}
void MemoryObject::operator delete[](void* p, std::align_val_t align) noexcept
{
    Deallocate(p, align);
}

void* MemoryObject::operator new  (size_t size, std::align_val_t al) noexcept
{
    return Allocate(size);
}

void* MemoryObject::operator new[](size_t size, std::align_val_t al) noexcept
{
    return Allocate(size);
}

void* MemoryObject::Allocate(size_t size)
{
    return MemoryUtilities::Allocate(size);
}

void* MemoryObject::Allocate(size_t size, std::align_val_t align)
{
    return MemoryUtilities::Allocate(size, static_cast<size_t>(align));
}
void MemoryObject::Deallocate(void* p)
{
    MemoryUtilities::Deallocate(p);
}

void MemoryObject::Deallocate(void* p, size_t size)
{
    MemoryUtilities::Deallocate(p);
}

void MemoryObject::Deallocate(void* p, std::align_val_t align)
{
    MemoryUtilities::Deallocate(p, static_cast<size_t>(align));
}

void MemoryObject::Deallocate(void* p, size_t size, std::align_val_t align)
{
    MemoryUtilities::Deallocate(p, static_cast<size_t>(align));
}
}
}
