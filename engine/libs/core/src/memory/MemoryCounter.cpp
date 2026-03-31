#include "core/memory/MemoryCounter.hpp"
#include "core/debug/Assert.hpp"
#include "core/thread/Thread.hpp"
#include <string>

namespace slug::core
{

MemoryCounter::MemoryCounter(bool debugPrint)
    : m_debugPrint(debugPrint)
{
    size_t count = std::max(std::strlen(s_DefaultLabel), s_LabelSize);
    strncpy_s(m_item.label, s_DefaultLabel, count);
}

void MemoryCounter::IncrementMemorySize(size_t size)
{
    const size_t oldSize = m_item.size.fetch_add(size, std::memory_order_relaxed);
    const size_t newSize = oldSize + size;

    if (m_debugPrint)
    {
        printf("allocate : %llu + %llu = %llu\n", oldSize, size, newSize);
    }
}

void MemoryCounter::DecrementMemorySize(size_t size)
{
    if (size == 0) return;

    const size_t oldSize = m_item.size.fetch_sub(size, std::memory_order_relaxed);

    if (oldSize < size)
    {
        m_item.size.fetch_add(size, std::memory_order_relaxed); // rollback
        SLUG_THROW_EXCEPTION("Error Memory Counter Deallocate");
    }

    if (m_debugPrint)
    {
        std::printf("deallocate : %zu - %zu = %zu\n", oldSize, size, oldSize - size);
    }
}

size_t MemoryCounter::GetCurrentMemorySize()
{
    return m_item.size;
}

void MemoryCounter::Print()
{
    printf("[%s] memory size : %llu\n", m_item.label, m_item.size.load());
}

}
