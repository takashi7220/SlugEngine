#include "core/memory/MemoryUtility.hpp"
#include "core/memory/debug/MemoryDebugUtility.hpp"
#include <memory>

#if defined(SLUG_USE_MIMALLOC)
#include <mimalloc.h>
#include <mimalloc-stats.h>
#endif

namespace slug::core
{

inline void* new_internal(size_t size)
{
#if defined(SLUG_USE_MIMALLOC)
    return mi_new(size);
#else
    return malloc(size);
#endif
}

inline void* new_aligned_internal(size_t size, size_t align)
{
#if defined(SLUG_USE_MIMALLOC)
    return mi_new_aligned(size, align);
#else
    return _aligned_malloc(size, align);
#endif
}

inline void free_internal(void* p)
{
#if defined(SLUG_USE_MIMALLOC)
    mi_free(p);
#else
    return free(p);
#endif
}

inline void free_aligned_inernal(void* p, size_t align)
{
#if defined(SLUG_USE_MIMALLOC)
    mi_free_aligned(p, align);
#else
    return _aligned_free(p);
#endif
}

void* MemoryUtility::Allocate(size_t size)
{
#if SLUG_DEBUG_MODE
    size_t totalSize = size + sizeof(MemoryHeader);
    void* raw = new_internal(totalSize);
    MemoryHeader* header = static_cast<MemoryHeader*>(raw);
    header->size = size;
    header->label = MemoryLabelScope::Current();
    header->subLabel = MemorySubLabelScope::Current();
    MemoryDebugUtility::RecordAllocate(raw, *header);
    return reinterpret_cast<void*>(header + 1);
#else
    return new_internal(size);
#endif
}

void* MemoryUtility::Allocate(size_t size, size_t align)
{
#if SLUG_DEBUG_MODE
    size_t totalSize = size + sizeof(MemoryHeader);
    void* raw = new_aligned_internal(totalSize, align);
    MemoryHeader* header = static_cast<MemoryHeader*>(raw);
    header->size = size;
    header->label = MemoryLabel(MemoryLabelScope::Current());
    header->subLabel = MemoryLabel(MemorySubLabelScope::Current());
    MemoryDebugUtility::RecordAllocate(raw, *header);
    return reinterpret_cast<void*>(header + 1);
#else
    return new_aligned_internal(size, align);
#endif
}

void MemoryUtility::Deallocate(void* p)
{
    if (!p)
    {
        return;
    }
#if SLUG_DEBUG_MODE
    MemoryHeader* header = reinterpret_cast<MemoryHeader*>(p) - 1;
    MemoryDebugUtility::RecordDeallocate(p, *header);
    free_internal(header);
#else
    free_internal(p);
#endif
}

void MemoryUtility::Deallocate(void* p, size_t align)
{
    if (!p)
    {
        return;
    }
#if SLUG_DEBUG_MODE
    MemoryHeader* header = reinterpret_cast<MemoryHeader*>(p) - 1;
    MemoryDebugUtility::RecordDeallocate(p, *header);
    free_aligned_inernal(header, align);
#else
    free_aligned_inernal(p, align);
#endif
}

void MemoryUtility::Memset(void* dst, int32_t value, size_t dstSize)
{
    memset(dst, value, dstSize);
}

void MemoryUtility::Memcpy(void* dst, size_t dstSize, const void* src, size_t srcSize)
{
    memcpy_s(dst, dstSize, src, srcSize);
}

MemoryProfile MemoryUtility::GetCurrentProfile()
{
    MemoryProfile ret = {};
#if defined(SLUG_USE_MIMALLOC)
    mi_stats_t stats = {};
    stats.size = sizeof(mi_stats_t);
    stats.version = MI_STAT_VERSION;

    if (!mi_stats_get(&stats))
    {
        return ret;
    }

    ret.malloc_requested_current = stats.malloc_requested.current;
    ret.malloc_requested_peak = stats.malloc_requested.peak;
    ret.commited_current = stats.committed.current;
    ret.reserved_current = stats.reserved.current;
#endif
    return ret;
}

void MemoryUtility::EnableDebugLayer()
{
#if defined(SLUG_USE_MIMALLOC)
    mi_option_enable(mi_option_verbose);
    mi_option_enable(mi_option_show_errors);
#endif
}
}
