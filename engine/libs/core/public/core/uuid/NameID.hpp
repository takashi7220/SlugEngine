#pragma once

#include "core/container/String.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/utility/Hash.hpp"
#include "core/thread/Atomic.hpp"

#define SLUG_NAME_ID_MAX_COUNT 512

namespace slug::core
{
class NameID
{
public:
    NameID();
    NameID(core::StringView name);
    bool operator==(const NameID& v) const;
    StringView GetStr() const;
    size_t GetHash() const;
private:
    struct Registry
    {
        FixedString<64> nameIDList[SLUG_NAME_ID_MAX_COUNT];
        core::TAtomic<size_t> currentNameIDCount = 0;
    };

    static Registry& GetRegistry();
    static int64_t GetOrAddNameIDIndex(StringView name);
    static StringView GetNameIDStr(int64_t id);
    static int64_t GetCurrentNameIDCount();
    static void IncrementCurrentNameIDCount();
    static void RegisterNameID(StringView name, int64_t id);
private:
    int64_t m_id;
};

struct NameIDHash
{
    size_t operator()(const NameID& nameId) const
    {
        return nameId.GetHash();
    }
};
}
