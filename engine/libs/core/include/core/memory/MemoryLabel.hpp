#pragma once

#include <cstdint>
namespace slug::core
{
using MemoryLabelId = uint8_t;
struct MemoryLabel
{
    MemoryLabelId id;

    constexpr MemoryLabel(MemoryLabelId _id = 0)
        : id(_id)
    {
    }

    constexpr operator MemoryLabelId() const
    {
        return id;
    }
};
}
