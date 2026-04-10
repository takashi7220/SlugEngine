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
    void* p = mi_new(size);
#if defined(DEBUG_MODE)
    MemoryDebugUtilities::RecordAllocate(p, MemoryLabel(MemoryLabelType::MemoryLabelType_System));
#endif
    return p;
}

void* MemoryUtilities::Allocate(size_t size, size_t align)
{
    void *p = mi_new_aligned(size, align);
#if defined(DEBUG_MODE)
    MemoryDebugUtilities::RecordAllocate(p, MemoryLabel(MemoryLabelType::MemoryLabelType_System));
#endif
    return p;
}

void MemoryUtilities::Deallocate(void* p)
{
#if defined(DEBUG_MODE)
    MemoryDebugUtilities::RecordDeallocate(p, MemoryLabel(MemoryLabelType::MemoryLabelType_System));
#endif
    mi_free(p);
}

void MemoryUtilities::Deallocate(void* p, size_t align)
{
#if defined(DEBUG_MODE)
    MemoryDebugUtilities::RecordDeallocate(p, MemoryLabel(MemoryLabelType::MemoryLabelType_System));
#endif
    mi_free_aligned(p, align);
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
