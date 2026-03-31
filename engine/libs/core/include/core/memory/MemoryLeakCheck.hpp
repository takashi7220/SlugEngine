#pragma once

#include "core/thread/Atomic.hpp"

namespace slug::core
{

struct MemoryLabelTable
{
    static constexpr uint32_t MAX_LABEL_CAPACITY = 128;
    static constexpr uint32_t MAX_LABEL_STR_LENGTH = 128;

    struct Entry
    {
        core::TAtomic<uint64_t> key { 0 };
        core::TAtomic<long long> bytes { 0 };
        char label[MAX_LABEL_STR_LENGTH] = {};
    };

    static Entry& Slot(uint32_t i);
    static Entry* TryFindOrCreate(uint64_t key, const char* label);
    static void AddBytes(uint64_t key, const char* label, long long delta) noexcept;
    static long long LoadBytes(uint64_t key, const char* label) noexcept;
};

}
