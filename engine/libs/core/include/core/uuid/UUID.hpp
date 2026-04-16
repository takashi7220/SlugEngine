#pragma once

#include "core/container/String.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/utility/Hash.hpp"

namespace slug::core
{
class UUID
{
public:
    bool operator==(const UUID& v) const
    {
        return this->value[0] == v.value[0] && this->value[1] == v.value[1];
    }

    UUID()
    {
        InternalID id = GenerateInternal();
        value[0] = id.value[0];
        value[1] = id.value[1];
    }

    size_t GetHash() const
    {
        size_t seed = 0;
        HashUtility::HashCombine(seed, value[0]);
        HashUtility::HashCombine(seed, value[1]);
        return seed;
    }

    core::StringView Get() const
    {
        return str;
    }
private:

    struct InternalID
    {
        uint64_t value[2];
    };

    static InternalID GenerateInternal();

    union
    {
        uint64_t value[2];
        char str[16];
    };
};

struct UUIDHash
{
    size_t operator()(const UUID& uuid) const
    {
        return uuid.GetHash();
    }
};

}
