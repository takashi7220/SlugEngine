#include "core/memory/MemoryLeakCheck.hpp"

namespace slug::core
{
MemoryLabelTable::Entry& Slot(uint32_t i)
{
    static MemoryLabelTable::Entry s_entries[MemoryLabelTable::MAX_LABEL_CAPACITY];
    return s_entries[i];
}

MemoryLabelTable::Entry* TryFindOrCreate(uint64_t key, const char* label)
{
    uint32_t index = static_cast<uint32_t>(key) & (MemoryLabelTable::MAX_LABEL_CAPACITY - 1);

    for (uint32_t probe = 0; probe <MemoryLabelTable::MAX_LABEL_CAPACITY; ++probe)
    {
        MemoryLabelTable::Entry& e = Slot((index + probe) & (MemoryLabelTable::MAX_LABEL_CAPACITY - 1));
        uint64_t cur = e.key.load(std::memory_order_acquire);

        if (cur == key)
        {
            return &e;
        }

        if (cur == 0)
        {
            uint64_t expected = 0;
            if (e.key.compare_exchange_strong(expected, key, std::memory_order_acq_rel, std::memory_order_relaxed))
            {
                std::strncpy(e.label, label, MemoryLabelTable::MAX_LABEL_STR_LENGTH);
                e.label[MemoryLabelTable::MAX_LABEL_STR_LENGTH - 1] = '\0';
                return &e;
            }
        }
    }
    return nullptr;
}

void MemoryLabelTable::AddBytes(uint64_t key, const char* label, long long delta) noexcept
{
    MemoryLabelTable::Entry* e = TryFindOrCreate(key, label);
    if (!e)
    {
        return;
    }
    e->bytes.fetch_add(delta, std::memory_order_relaxed);
}

long long MemoryLabelTable::LoadBytes(uint64_t key, const char* label) noexcept
{
    MemoryLabelTable::Entry* e = TryFindOrCreate(key, label);
    if (!e)
    {
        return 0;
    }
    return e->bytes.load(std::memory_order_relaxed);
}
}
