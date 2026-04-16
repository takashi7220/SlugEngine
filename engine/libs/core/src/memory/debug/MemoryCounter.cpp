#if defined(DEBUG_MODE)
#include "core/memory/debug/MemoryCounter.hpp"
#include "core/debug/Assert.hpp"
#include "core/thread/Thread.hpp"
#include <string>

namespace slug::core
{

size_t MemoryLabelTable::IncrementBytes(MemoryLabel label, size_t size) noexcept
{
    MemoryLabelTable::Entry* e = TryFindOrCreate(label);
    if (!e)
    {
        return 0;
    }
    return e->bytes.fetch_add(size, std::memory_order_relaxed);
}

size_t MemoryLabelTable::DecrementBytes(MemoryLabel label, size_t size) noexcept
{
    MemoryLabelTable::Entry* e = TryFindOrCreate(label);
    if (!e)
    {
        return 0;
    }
    return e->bytes.fetch_sub(size, std::memory_order_relaxed);
}

size_t MemoryLabelTable::GetBytes(MemoryLabel label) noexcept
{
    MemoryLabelTable::Entry* e = TryFindOrCreate(label);
    if (!e)
    {
        return 0;
    }
    return e->bytes.load(std::memory_order_relaxed);
}

size_t MemoryLabelTable::GetTotalBytes() noexcept
{
    size_t totalSize = 0;
    for (size_t i = 0; i < m_currentEntryCount.load(); i++)
    {
        totalSize += m_entries[i].bytes.load();
    }
    return totalSize;
}

MemoryLabelTable::Entry& MemoryLabelTable::Slot(uint32_t i)
{
    return m_entries[i];
}

MemoryLabelTable::Entry* MemoryLabelTable::TryFindOrCreate(MemoryLabel label)
{
    size_t currentSize = m_currentEntryCount.load();
    for (size_t i = 0; i < currentSize; ++i)
    {
        Entry& entry = Slot(static_cast<uint32_t>(i));
        if (entry.label == label)
        {
            return &entry;
        }
    }

    if (m_currentEntryCount.load() < SLUG_MEMORY_LABEL_CAPACITY)
    {
        size_t createIndex = m_currentEntryCount.fetch_add(1);
        Entry& entry = Slot(static_cast<uint32_t>(createIndex));
        entry.label = label;
        return &entry;
    }
    return nullptr;
}

MemoryCounter::MemoryCounter(bool debugPrint)
    : m_debugPrint(debugPrint)
{
}

void MemoryCounter::IncrementMemorySize(size_t size, MemoryLabel label)
{
    const size_t oldSize = m_memoryLabelTable.IncrementBytes(label, size);
    if (m_debugPrint)
    {
        const size_t newSize = oldSize + size;
        printf("allocate : %llu + %llu = %llu\n", oldSize, size, newSize);
    }
}

void MemoryCounter::DecrementMemorySize(size_t size, MemoryLabel label)
{
    if (size == 0)
    {
        return;
    }

    const size_t oldSize = m_memoryLabelTable.DecrementBytes(label, size);
    if (oldSize < size)
    {
        SLUG_THROW_EXCEPTION("Error Memory Counter Deallocate");
    }

    if (m_debugPrint)
    {
        std::printf("deallocate : %zu - %zu = %zu\n", oldSize, size, oldSize - size);
    }
}

size_t MemoryCounter::GetCurrentMemorySize(MemoryLabel label)
{
    return m_memoryLabelTable.GetBytes(label);
}

size_t MemoryCounter::GetCurrentTotalMemorySize()
{
    return m_memoryLabelTable.GetTotalBytes();
}

}
#endif
