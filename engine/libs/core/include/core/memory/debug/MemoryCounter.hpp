#pragma once

#if defined(DEBUG_MODE)
#include "core/thread/Atomic.hpp"
#include "core/memory/MemoryLabel.hpp"

#define SLUG_MEMORY_LABEL_CAPACITY 256
#define SLUG_MEMORY_LABEL_NAME_LENGTH 128

namespace slug::core
{
struct MemoryLabelTable
{
    size_t IncrementBytes(MemoryLabel label, size_t size) noexcept;
    size_t DecrementBytes(MemoryLabel label, size_t size) noexcept;
    size_t GetBytes(MemoryLabel label) noexcept;
    size_t GetTotalBytes() noexcept;
private:
    struct Entry
    {
        MemoryLabel label = {};
        core::TAtomic<size_t> bytes { 0 };
    };

private:
    Entry& Slot(uint32_t i);
    Entry* TryFindOrCreate(MemoryLabel label);

private:
    Entry m_entries[SLUG_MEMORY_LABEL_CAPACITY] = {};
    core::TAtomic<size_t> m_currentEntryCount = 0;
};

class MemoryCounter
{
public:
    MemoryCounter(bool debugPrint = false);
    void IncrementMemorySize(size_t size, MemoryLabel label);
    void DecrementMemorySize(size_t size, MemoryLabel label);
    size_t GetCurrentMemorySize(MemoryLabel label);
    size_t GetCurrentTotalMemorySize();

private:
    MemoryLabelTable m_memoryLabelTable = {};
    bool m_debugPrint = false;
};
}
#endif
