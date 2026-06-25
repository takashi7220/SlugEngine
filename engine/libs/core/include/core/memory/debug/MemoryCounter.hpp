#pragma once

#include "core/Define.hpp"

#if SLUG_ENABLE_MEMORY_COUNTER
#include "core/thread/Atomic.hpp"
#include "core/memory/MemoryLabel.hpp"

#define SLUG_MEMORY_LABEL_CAPACITY 256
#define SLUG_MEMORY_LABEL_NAME_LENGTH 128

namespace slug::core
{
struct MemoryLabelTable
{
    struct Key
    {
        const MemoryLabel& label;
        const MemoryLabel& subLabel;

        Key(const MemoryLabel& label, const MemoryLabel& subLabel)
            : label(label)
            , subLabel(subLabel)
        {
        }
    };
    size_t IncrementBytes(const Key& key, size_t size) noexcept;
    size_t DecrementBytes(const Key& key, size_t size) noexcept;
    size_t GetBytes(const Key& key) noexcept;
    size_t GetTotalBytes() noexcept;
private:
    struct Entry
    {
        MemoryLabel label = {};
        MemoryLabel subLabel = {};
        core::TAtomic<size_t> bytes { 0 };
    };

private:
    Entry& Slot(uint32_t i);
    Entry* TryFindOrCreate(const Key& key);

private:
    Entry m_entries[SLUG_MEMORY_LABEL_CAPACITY] = {};
    core::TAtomic<size_t> m_currentEntryCount = 0;
};

class MemoryCounter
{
public:
    MemoryCounter(bool debugPrint = false);
    void IncrementMemorySize(size_t size, const MemoryLabel& label, const MemoryLabel& subLabel);
    void DecrementMemorySize(size_t size, const MemoryLabel& label, const MemoryLabel& subLabel);
    size_t GetCurrentMemorySize(const MemoryLabel& label, const MemoryLabel& subLabel);
    size_t GetCurrentTotalMemorySize();

private:
    MemoryLabelTable m_memoryLabelTable = {};
    bool m_debugPrint = false;
};
}
#endif
