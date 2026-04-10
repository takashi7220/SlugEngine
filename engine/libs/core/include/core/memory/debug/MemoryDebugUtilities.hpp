#pragma once



#if defined(DEBUG_MODE)

#include "core/memory/debug/MemoryCounter.hpp"

namespace slug::core
{

struct MemoryProfileInfo
{
    size_t currentUseMemorySize = 0;
    size_t availableUseMemorySize = 0;
    size_t currentCommitMemorySize = 0;
    size_t peakCommitMemorySize = 0;

    bool operator==(const MemoryProfileInfo& v)
    {
        bool same = true;
        same &= (this->currentUseMemorySize == v.currentUseMemorySize);
        same &= (this->availableUseMemorySize == v.availableUseMemorySize);
        same &= (this->currentCommitMemorySize == v.currentUseMemorySize);
        same &= (this->peakCommitMemorySize == v.peakCommitMemorySize);
        return same;
    }
};

using MemoryLeakCheckCallback = void(*)(size_t leakMemorySize, MemoryLabel label);
class MemoryLeakCheckScope final
{
public:
    MemoryLeakCheckScope(MemoryLabel label);
    ~MemoryLeakCheckScope();

private:
    size_t m_checkSize;
    MemoryLabel m_label;
};
class MemoryDebugUtilities
{
public:
    static MemoryProfileInfo GetMemoryProfileInfo();
    static void SetMemoryCounter(MemoryCounter* counter);
    static void SetMemoryLeackCheckCallback(MemoryLeakCheckCallback callback);
    static size_t GetCurrentMemorySize(MemoryLabel label);
    static void RegisterMemoryLabelName(MemoryLabel label, const char* name);
    static const char* GetMemoryLabelName(MemoryLabel label);
    static void RecordAllocate(void* p, MemoryLabel label);
    static void RecordDeallocate(void* p, MemoryLabel label);
};
}
#endif
