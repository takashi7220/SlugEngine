#pragma once

#include "core/utility/StringUtility.hpp"

#define SLUG_MEMORY_LABEL_LENGTH 64

namespace slug::core
{
struct MemoryLabel
{
    MemoryLabel(const char* str = "")
        : name("")
    {
        StringUtility::Strncpy(name, str, SLUG_MEMORY_LABEL_LENGTH);
    }

    bool operator ==(const MemoryLabel& v)
    {
        return StringUtility::IsEqual(name, v.name);
    }

    char name[SLUG_MEMORY_LABEL_LENGTH] = "";
};

struct MemoryHeader
{
    size_t size;
    MemoryLabel label;
    MemoryLabel subLabel;
};
}
