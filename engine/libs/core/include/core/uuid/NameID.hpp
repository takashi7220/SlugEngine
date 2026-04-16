#pragma once

#include "core/container/String.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/utility/Hash.hpp"

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
