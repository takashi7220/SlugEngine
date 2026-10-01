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

struct alignas(double) MemoryHeader
{
    // 確保ブロック先頭から利用者ポインタまでのバイト数。解放時に先頭へ戻すために保持する。
    size_t offset;
    size_t size;
    MemoryLabel label;
    MemoryLabel subLabel;
};
}
