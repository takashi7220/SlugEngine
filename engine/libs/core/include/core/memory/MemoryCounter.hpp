#pragma once

#include "core/thread/Atomic.hpp"

#define SLUG_MEMORY_LABEL_CAPACITY 256
#define SLUG_MEMORY_LABLE_STR_LENGTH 128

namespace slug::core
{

struct MemoryLabelTable
{
    size_t IncrementBytes(const char* label, size_t size) noexcept;
    size_t DecrementBytes(const char* label, size_t size) noexcept;
    size_t GetBytes(const char* label) noexcept;

private:
    struct Entry
    {
        char label[SLUG_MEMORY_LABLE_STR_LENGTH] = {};
        core::TAtomic<size_t> bytes { 0 };
    };

private:
    Entry& Slot(uint32_t i);
    Entry* TryFindOrCreate(const char* label);

private:
    Entry m_entries[SLUG_MEMORY_LABEL_CAPACITY] = {};
    core::TAtomic<size_t> m_currentEntryCount = 0;
};

class MemoryCounter
{
public:
    MemoryCounter(bool debugPrint = false);
    void IncrementMemorySize(size_t size, const char* label);
    void DecrementMemorySize(size_t size, const char* label);
    size_t GetCurrentMemorySize(const char* label);
    void Print();

private:
    MemoryLabelTable m_memoryLabelTable = {};
    bool m_debugPrint = false;
};
}
