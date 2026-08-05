#include "core/Define.hpp"

#if SLUG_ENABLE_MEMORY_COUNTER
#include "core/memory/debug/MemoryDebugUtility.hpp"
#include "core/container/String.hpp"
#include "core/debug/Exception.hpp"
#if defined(SLUG_USE_MIMALLOC)
#include <mimalloc.h>
#endif
#include <memory>

namespace slug::core
{

inline size_t usable_size_internal(void* p)
{
#if defined(SLUG_USE_MIMALLOC)
    return mi_usable_size(p);
#else
    return _msize(p);
#endif
}

MemoryLabelScope::MemoryLabelScope(MemoryLabel newLabel)
    : m_prevLabel(m_currentLabel)
{
    m_currentLabel = newLabel;
}

MemoryLabelScope::~MemoryLabelScope()
{
    m_currentLabel = m_prevLabel;
}

const MemoryLabel& MemoryLabelScope::Current()
{
    return m_currentLabel;
}

MemorySubLabelScope::MemorySubLabelScope(MemoryLabel newLabel)
    : m_prevLabel(m_currentLabel)
{
    m_currentLabel = newLabel;
}

MemorySubLabelScope::~MemorySubLabelScope()
{
    m_currentLabel = m_prevLabel;
}

const MemoryLabel& MemorySubLabelScope::Current()
{
    return m_currentLabel;
}

MemoryLeakCheckScope::MemoryLeakCheckScope(MemoryLabel label, MemoryLabel subLabel)
    : m_checkSize(0)
    , m_label(label)
    , m_subLabel(subLabel)
{
    m_checkSize = MemoryDebugUtility::GetCurrentMemorySize(m_label, m_subLabel);
}

MemoryLeakCheckScope::~MemoryLeakCheckScope()
{
    size_t currentSize = MemoryDebugUtility::GetCurrentMemorySize(m_label, m_subLabel);
    if (m_checkSize != currentSize)
    {
        if (MemoryDebugUtility::GetMemoryLeakCheckCallback())
        {
            MemoryDebugUtility::GetMemoryLeakCheckCallback()(currentSize - m_checkSize, m_label);
        }
        else
        {
            printf("Error. A memory leak was detected : %llu byte, [%s:%s]\n", currentSize - m_checkSize, m_label.name, m_subLabel.name);
        }
    }
}

thread_local MemoryLabel MemoryLabelScope::m_currentLabel = MemoryLabel("System");
thread_local MemoryLabel MemorySubLabelScope::m_currentLabel = MemoryLabel("Default");

MemoryProfileInfo MemoryDebugUtility::GetMemoryProfileInfo()
{
    size_t currentRss = 0;
    size_t currentCommit = 0;
    size_t peakCommit = 0;

#if defined(SLUG_USE_MIMALLOC)
    size_t pageFaults = 0;
    size_t peakRss = 0;
    size_t elapsedMsecs = 0;
    size_t userMsecs = 0;
    size_t systemMsecs = 0;

    mi_process_info(&elapsedMsecs, &userMsecs, &systemMsecs, &currentRss, &peakRss, &currentCommit, &peakCommit, &pageFaults);
#endif

    MemoryProfileInfo memInfo = {};
    memInfo.currentUseMemorySize = currentRss;
    memInfo.availableUseMemorySize = peakCommit;
    memInfo.currentCommitMemorySize = currentCommit;
    memInfo.peakCommitMemorySize = peakCommit;
    return memInfo;
}

size_t MemoryDebugUtility::GetCurrentMemorySize(const MemoryLabel& label, const MemoryLabel& subLabel)
{
    if (GetMemoryCounter())
    {
        return GetMemoryCounter()->GetCurrentMemorySize(label, subLabel);
    }
    return 0;
}

void MemoryDebugUtility::RecordAllocate(void* p, const MemoryHeader& header)
{
    if (GetMemoryCounter())
    {
        size_t size = usable_size_internal(p);
        GetMemoryCounter()->IncrementMemorySize(size, header.label, header.subLabel);
    }
}

void MemoryDebugUtility::RecordDeallocate(void* p, const MemoryHeader& header)
{
    if (GetMemoryCounter())
    {
        size_t size = usable_size_internal(p);
        GetMemoryCounter()->DecrementMemorySize(size, header.label, header.subLabel);
    }
}

void MemoryDebugUtility::BindMemoryCounter(MemoryCounter* counter)
{
    GetRegistry().memoryCounter = counter;
}

void MemoryDebugUtility::UnbindMemoryCounter()
{
    GetRegistry().memoryCounter = nullptr;
}

void MemoryDebugUtility::BindMemoryLeakCheckCallback(MemoryLeakCheckCallback callback)
{
    GetRegistry().memoryLeakCheckCallback = callback;
}

void MemoryDebugUtility::UnbindMemoryLeakCheckCallback()
{
    GetRegistry().memoryLeakCheckCallback = nullptr;
}

MemoryDebugUtility::Registry& MemoryDebugUtility::GetRegistry()
{
    static Registry registry;
    return registry;
}

MemoryCounter* MemoryDebugUtility::GetMemoryCounter()
{
    return GetRegistry().memoryCounter;
}

MemoryLeakCheckCallback MemoryDebugUtility::GetMemoryLeakCheckCallback()
{
    return GetRegistry().memoryLeakCheckCallback;
}
}
#endif
