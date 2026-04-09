#include "core/memory/MemoryUtilities.hpp"
#include "core/debug/Assert.hpp"
#include "core/debug/Exception.hpp"
#include <mimalloc.h>
#include <memory>

namespace slug::core
{

static MemoryCounter* s_MemoryCounter = nullptr;
static MemoryLeakCheckCallback s_MemoryLeakCheckCallback = nullptr;

MemoryLeakCheckScope::MemoryLeakCheckScope(const char* label)
    : m_checkSize(0)
    , m_label()
    , m_callback(s_MemoryLeakCheckCallback)
{
    m_checkSize = MemoryUtilities::GetCurrentMemorySize(SLUG_MEMORY_SYSTEM_LABEL);
    StringUtility::Strncpy(m_label, SLUG_MEMORY_SYSTEM_LABEL, SLUG_MEMORY_LABLE_STR_LENGTH);
}

MemoryLeakCheckScope::~MemoryLeakCheckScope()
{
    size_t currentSize = MemoryUtilities::GetCurrentMemorySize(m_label);
    if (m_checkSize != currentSize)
    {
        if (m_callback)
        {
            m_callback(currentSize - m_checkSize, m_label);
        }
        else
        {
            printf("Error. A memory leak was detected : %llu byte, %s\n", currentSize - m_checkSize, m_label);
        }
    }
}

void* MemoryUtilities::Allocate(size_t size, const char* label)
{
    void* p = mi_new(size);
    RecordAllocate(p, label);
    return p;
}

void* MemoryUtilities::Allocate(size_t size, size_t align, const char* label)
{
    void *p = mi_new_aligned(size, align);
    RecordAllocate(p, label);
    return p;
}

void MemoryUtilities::Deallocate(void* p, const char* label)
{
    RecordDeallocate(p, label);
    mi_free(p);
}

void MemoryUtilities::Deallocate(void* p, size_t align, const char* label)
{
    RecordDeallocate(p, label);
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

MemoryProfileInfo MemoryUtilities::GetMemoryProfileInfo()
{
    size_t elapsedMsecs = 0;
    size_t userMsecs = 0;
    size_t systemMsecs = 0;
    size_t currentRss = 0;
    size_t peakRss = 0;
    size_t currentCommit = 0;
    size_t peakCommit = 0;
    size_t pageFaults = 0;
    mi_process_info(&elapsedMsecs, &userMsecs, &systemMsecs, &currentRss, &peakRss, &currentCommit, &peakCommit, &pageFaults);

    MemoryProfileInfo memInfo = {};
    memInfo.currentUseMemorySize = currentRss;
    memInfo.availableUseMemorySize = peakCommit;
    memInfo.currentCommitMemorySize = currentCommit;
    memInfo.peakCommitMemorySize = peakCommit;
    return memInfo;
}

void MemoryUtilities::SetMemoryCounter(MemoryCounter* counter)
{
    s_MemoryCounter = counter;
}

void MemoryUtilities::SetMemoryLeackCheckCallback(MemoryLeakCheckCallback callback)
{
    s_MemoryLeakCheckCallback = callback;
}

size_t MemoryUtilities::GetCurrentMemorySize(const char* label)
{
    if (s_MemoryCounter)
    {
        return s_MemoryCounter->GetCurrentMemorySize(label);
    }
    return 0;
}

void MemoryUtilities::RecordAllocate(void* p, const char* label)
{
#if defined(DEBUG_MODE)
    if (s_MemoryCounter)
    {
        size_t size = mi_usable_size(p);
        s_MemoryCounter->IncrementMemorySize(size, label);
    }
#endif
}

void MemoryUtilities::RecordDeallocate(void* p, const char* label)
{
#if defined(DEBUG_MODE)
    if (s_MemoryCounter)
    {
        size_t size = mi_usable_size(p);
        s_MemoryCounter->DecrementMemorySize(size, label);
    }
#endif
}
}
