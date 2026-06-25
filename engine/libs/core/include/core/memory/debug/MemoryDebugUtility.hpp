#pragma once

#include "core/Define.hpp"

#if SLUG_ENABLE_MEMORY_COUNTER
#include "core/memory/debug/MemoryCounter.hpp"
#include "core/container/IntrusiveLinkedList.hpp"

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

class MemoryLabelScope final
{
public:
    explicit MemoryLabelScope(MemoryLabel newLabel);
    ~MemoryLabelScope();
    MemoryLabelScope(const MemoryLabelScope&) = delete;
    MemoryLabelScope& operator=(const MemoryLabelScope&) = delete;
    static const MemoryLabel& Current();

private:
    MemoryLabel m_prevLabel;
    static thread_local MemoryLabel m_currentLabel;
};

class MemorySubLabelScope final
{
public:
    explicit MemorySubLabelScope(MemoryLabel newLabel);
    ~MemorySubLabelScope();
    MemorySubLabelScope(const MemorySubLabelScope&) = delete;
    MemorySubLabelScope& operator=(const MemorySubLabelScope&) = delete;
    static const MemoryLabel& Current();

private:
    MemoryLabel m_prevLabel;
    static thread_local MemoryLabel m_currentLabel;
};

class MemoryLeakCheckScope final
{
public:
    MemoryLeakCheckScope(MemoryLabel label, MemoryLabel subLabel);
    ~MemoryLeakCheckScope();

private:
    size_t m_checkSize;
    MemoryLabel m_label;
    MemoryLabel m_subLabel;
};


class MemoryDebugUtility
{
public:
    struct MemoryLabelNamePair
    {
        MemoryLabel label = 0;
        char name[SLUG_MEMORY_LABEL_NAME_LENGTH];
    };
    using LinkedList = IntrusiveLinkedList<MemoryLabelNamePair>;
    using Node = LinkedList::Node;

    static MemoryProfileInfo GetMemoryProfileInfo();
    static size_t GetCurrentMemorySize(const MemoryLabel& label, const MemoryLabel& subLabel);
    static void RecordAllocate(void* p, const MemoryHeader& header);
    static void RecordDeallocate(void* p, const MemoryHeader& header);
    static void BindMemoryCounter(MemoryCounter* counter);
    static void UnbindMemoryCounter() ;
    static void BindMemoryLeakCheckCallback(MemoryLeakCheckCallback callback);
    static void UnbindMemoryLeakCheckCallback(); 
    static MemoryCounter* GetMemoryCounter();
    static MemoryLeakCheckCallback GetMemoryLeakCheckCallback();
private:

    struct Registry
    {
        MemoryCounter* memoryCounter = nullptr;
        MemoryLeakCheckCallback memoryLeakCheckCallback = nullptr;
    };

private:
    static Registry& GetRegistry();
};
}
#endif
