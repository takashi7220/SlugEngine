#include "core/uuid/NameID.hpp"
#include "core/container/Vector.hpp"
#include "core/debug/Assert.hpp"
namespace slug::core
{
NameID::NameID()
    : m_id(0)
{
    int64_t currentCount = GetCurrentNameIDCount();
    if (currentCount == 0)
    {
        RegisterNameID("Default", currentCount);
        IncrementCurrentNameIDCount();
    }
}

NameID::NameID(core::StringView name)
    : m_id(0)
{
    m_id = GetOrAddNameIDIndex(name);
}

bool NameID::operator==(const NameID& v) const
{
    return m_id == v.m_id;
}

StringView NameID::GetStr() const
{
    return GetNameIDStr(m_id);
}

size_t NameID::GetHash() const
{
    size_t seed = 0;
    HashUtility::HashCombine(seed, m_id);
    return seed;
}

NameID::Registry& NameID::GetRegistry()
{
    static Registry registry;
    return registry;
}

int64_t NameID::GetOrAddNameIDIndex(StringView name)
{
    int64_t currentCount = GetCurrentNameIDCount();
    for (int64_t i = 0; i < currentCount; i++)
    {
        if (GetRegistry().nameIDList[i] == name)
        {
            return static_cast<int64_t>(i);
        }
    }

    if (currentCount < SLUG_NAME_ID_MAX_COUNT)
    {
        RegisterNameID(name, currentCount);
        IncrementCurrentNameIDCount();
        return currentCount;
    }

    SLUG_ASSERT_MSG(false, "Error NameID Count Limit...");
    return -1;
}
StringView NameID::GetNameIDStr(int64_t id)
{
    if (id >= 0 && id < SLUG_NAME_ID_MAX_COUNT)
    {
        return GetRegistry().nameIDList[id].data();
    }

    SLUG_ASSERT_MSG(false, "Error, Invaild NameID");
    return "___Failed_NameID";

}

int64_t NameID::GetCurrentNameIDCount()
{
    return static_cast<int64_t>(GetRegistry().currentNameIDCount.load());
}

void NameID::IncrementCurrentNameIDCount()
{
    GetRegistry().currentNameIDCount.fetch_add(1);
}

void NameID::RegisterNameID(StringView name, int64_t id)
{
    GetRegistry().nameIDList[id] = name;
}
}
