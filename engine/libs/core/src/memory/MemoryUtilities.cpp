#include "core/memory/MemoryUtilities.hpp"
#include <mimalloc.h>
#include <memory>

#if defined(DEBUG_MODE)
#include "core/memory/debug/MemoryDebugUtilities.hpp"
#endif

namespace slug::core
{

void* MemoryUtilities::Allocate(size_t size)
{
    size_t totalSize = size + sizeof(MemoryHeader);
    void* raw = mi_new(totalSize);
    MemoryHeader* header = static_cast<MemoryHeader*>(raw);
    header->size = size;
    header->label = MemoryLabel(MemoryLabelScope::Current());
#if defined(DEBUG_MODE)
    MemoryDebugUtilities::RecordAllocate(raw, header->label);
#endif
    return reinterpret_cast<void*>(header + 1);
}

void* MemoryUtilities::Allocate(size_t size, size_t align)
{
    size_t totalSize = size + sizeof(MemoryHeader);
    void* raw = mi_new_aligned(totalSize, align);
    MemoryHeader* header = static_cast<MemoryHeader*>(raw);
    header->size = size;
    header->label = MemoryLabel(MemoryLabelScope::Current());
#if defined(DEBUG_MODE)
    MemoryDebugUtilities::RecordAllocate(raw, header->label);
#endif
    return reinterpret_cast<void*>(header + 1);
}

void MemoryUtilities::Deallocate(void* p)
{
    if (!p)
    {
        return;
    }
    MemoryHeader* header = reinterpret_cast<MemoryHeader*>(p) - 1;
#if defined(DEBUG_MODE)
    MemoryDebugUtilities::RecordDeallocate(p, header->label);
#endif
    mi_free(header);
}

void MemoryUtilities::Deallocate(void* p, size_t align)
{
    if (!p)
    {
        return;
    }
    MemoryHeader* header = reinterpret_cast<MemoryHeader*>(p) - 1;
#if defined(DEBUG_MODE)
    MemoryDebugUtilities::RecordDeallocate(p, header->label);
#endif
    mi_free_aligned(header, align);
}

void MemoryUtilities::Memset(void* dst, int32_t value, size_t dstSize)
{
    memset(dst, value, dstSize);
}

void MemoryUtilities::Memcpy(void* dst, size_t dstSize, const void* src, size_t srcSize)
{
    memcpy_s(dst, dstSize, src, srcSize);
}
}
