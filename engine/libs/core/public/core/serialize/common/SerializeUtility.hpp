#pragma once

#include <cstdint>
#include "core/serialize/common/SerializeTraits.hpp"

namespace slug::core
{
struct SerializeHeader
{
    uint32_t magic = 0;
    uint32_t version = 0;
    uint64_t reserved = 0;
};

class SerializeUtility
{
public:
    inline size_t GetHeaderSize()
    {
        return sizeof(SerializeHeader);
    }
};
}
