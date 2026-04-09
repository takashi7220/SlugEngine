#include "core/memory/MemoryCounter.hpp"
#include "core/debug/Assert.hpp"
#include "core/thread/Thread.hpp"
#include <string>

namespace slug::core
{

size_t MemoryLabelTable::IncrementBytes(const char* label, size_t size) noexcept
{
    MemoryLabelTable::Entry* e = TryFindOrCreate(label);
    if (!e)
    {
        return 0;
    }
    return e->bytes.fetch_add(size, std::memory_order_relaxed);
}

size_t MemoryLabelTable::DecrementBytes(const char* label, size_t size) noexcept
{
    MemoryLabelTable::Entry* e = TryFindOrCreate(label);
    if (!e)
    {
        return 0;
    }
    return e->bytes.fetch_sub(size, std::memory_order_relaxed);
}

size_t MemoryLabelTable::GetBytes(const char* label) noexcept
{
    MemoryLabelTable::Entry* e = TryFindOrCreate(label);
    if (!e)
    {
        return 0;
    }
    return e->bytes.load(std::memory_order_relaxed);
}

MemoryLabelTable::Entry& MemoryLabelTable::Slot(uint32_t i)
{
    return m_entries[i];
}

MemoryLabelTable::Entry* MemoryLabelTable::TryFindOrCreate(const char* label)
{
    size_t currentSize = m_currentEntryCount.load();
    for (size_t i = 0; i < currentSize; ++i)
    {
        Entry& entry = Slot(static_cast<uint32_t>(i));
        if (StringUtility::Strcmp(entry.label, label) == 0)
        {
            return &entry;
        }
    }

    if (m_currentEntryCount.load() < SLUG_MEMORY_LABEL_CAPACITY)
    {
        size_t createIndex = m_currentEntryCount.fetch_add(1);
        Entry& entry = Slot(static_cast<uint32_t>(createIndex));
        StringUtility::Strncpy(entry.label, label, SLUG_MEMORY_LABLE_STR_LENGTH);
        return &entry;
    }
    return nullptr;
}

MemoryCounter::MemoryCounter(bool debugPrint)
    : m_debugPrint(debugPrint)
{
}

void MemoryCounter::IncrementMemorySize(size_t size, const char* label)
{
    const size_t oldSize = m_memoryLabelTable.IncrementBytes(label, size);
    if (m_debugPrint)
    {
        const size_t newSize = oldSize + size;
        printf("allocate : %llu + %llu = %llu\n", oldSize, size, newSize);
    }
}

void MemoryCounter::DecrementMemorySize(size_t size, const char* label)
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

size_t MemoryCounter::GetCurrentMemorySize(const char* label)
{
    return m_memoryLabelTable.GetBytes(label);
}

void MemoryCounter::Print()
{
}

}
