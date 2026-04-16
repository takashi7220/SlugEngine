#if defined(DEBUG_MODE)
#include "core/memory/debug/MemoryDebugUtilities.hpp"
#include "core/container/String.hpp"
#include "core/debug/Exception.hpp"
#include <mimalloc.h>
#include <memory>

namespace slug::core
{
static MemoryCounter* s_MemoryCounter = nullptr;
static MemoryLeakCheckCallback s_MemoryLeakCheckCallback = nullptr;
struct MemoryLabelNamePair
{
    MemoryLabel label = static_cast<MemoryLabelId>(MemoryLabelType_Unknown);
    char name[SLUG_MEMORY_LABEL_NAME_LENGTH];
};
struct MemoryLabelData
{
    MemoryLabelNamePair labelNamePairs[SLUG_MEMORY_LABEL_CAPACITY] = {};
    TAtomic<uint32_t> currentCount = 0;
};
static MemoryLabelData s_MemoryLabelData = {};

MemoryLeakCheckScope::MemoryLeakCheckScope(MemoryLabel label)
    : m_checkSize(0)
    , m_label()
{
    m_checkSize = MemoryDebugUtilities::GetCurrentMemorySize(label);
    m_label = label;
}

MemoryLeakCheckScope::~MemoryLeakCheckScope()
{
    size_t currentSize = MemoryDebugUtilities::GetCurrentMemorySize(m_label);
    if (m_checkSize != currentSize)
    {
        if (s_MemoryLeakCheckCallback)
        {
            s_MemoryLeakCheckCallback(currentSize - m_checkSize, m_label);
        }
        else
        {
            printf("Error. A memory leak was detected : %llu byte, %s\n", currentSize - m_checkSize, MemoryDebugUtilities::GetMemoryLabelName(m_label));
        }
    }
}

thread_local MemoryLabelId MemoryLabelScope::m_currentLabel = 0;


MemoryProfileInfo MemoryDebugUtilities::GetMemoryProfileInfo()
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

void MemoryDebugUtilities::SetMemoryCounter(MemoryCounter* counter)
{
    s_MemoryCounter = counter;
}

void MemoryDebugUtilities::SetMemoryLeackCheckCallback(MemoryLeakCheckCallback callback)
{
    s_MemoryLeakCheckCallback = callback;
}

size_t MemoryDebugUtilities::GetCurrentMemorySize(MemoryLabel label)
{
    if (s_MemoryCounter)
    {
        return s_MemoryCounter->GetCurrentMemorySize(label);
    }
    return 0;
}

void MemoryDebugUtilities::RegisterMemoryLabelName(MemoryLabel label, const char* name)
{
    bool isFound = false;
    uint32_t count = s_MemoryLabelData.currentCount.load();
    for (uint32_t i = 0; i < count; i++)
    {
        if (s_MemoryLabelData.labelNamePairs[i].label == label)
        {
            isFound = true;
            break;
        }
    }

    if (!isFound)
    {
        if (count < SLUG_MEMORY_LABEL_CAPACITY)
        {
            s_MemoryLabelData.labelNamePairs[count].label = label;
            core::StringUtility::Strncpy(s_MemoryLabelData.labelNamePairs[count].name, name, SLUG_MEMORY_LABEL_NAME_LENGTH);
            s_MemoryLabelData.currentCount.fetch_add(1);
        } else
        {
            SLUG_THROW_EXCEPTION("Error, memory label capacity exceeded.");
        }
    }
}

const char* MemoryDebugUtilities::GetMemoryLabelName(MemoryLabel label)
{
    uint32_t count = s_MemoryLabelData.currentCount.load();
    for (uint32_t i = 0; i < count; i++)
    {
        if (s_MemoryLabelData.labelNamePairs[i].label == label)
        {
            return s_MemoryLabelData.labelNamePairs[i].name;
        }
    }
    return "Unknown";
}


void MemoryDebugUtilities::RecordAllocate(void* p, MemoryLabel label)
{
    if (s_MemoryCounter)
    {
        size_t size = mi_usable_size(p);
        s_MemoryCounter->IncrementMemorySize(size, label);
    }
}

void MemoryDebugUtilities::RecordDeallocate(void* p, MemoryLabel label)
{
    if (s_MemoryCounter)
    {
        size_t size = mi_usable_size(p);
        s_MemoryCounter->DecrementMemorySize(size, label);
    }
}

}
#endif
