#pragma once

#include "core/thread/Atomic.hpp"

namespace slug::core
{

class MemoryCounter
{
public:
    static constexpr char s_DefaultLabel[] = "System";
    static constexpr size_t s_LabelSize = 256;

    struct CountItem
    {
        char label[s_LabelSize];
        TAtomic<size_t> size;
    };
public:
    MemoryCounter(bool debugPrint = false);
    void IncrementMemorySize(size_t size);
    void DecrementMemorySize(size_t size);
    size_t GetCurrentMemorySize();
    void Print();
private:
    CountItem m_item = {};
    bool m_debugPrint = false;
};
}
