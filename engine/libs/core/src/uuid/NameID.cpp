#include "core/uuid/NameID.hpp"
#include "core/container/Vector.hpp"
#include "core/debug/Assert.hpp"
namespace slug::core
{

static constexpr size_t s_NameIDMaxCount = 512;
static FixedString<64> s_NameIDList[s_NameIDMaxCount];
static size_t s_CurrentNameIDCount = 0;

int64_t GetOrAddNameIDIndex(StringView name)
{
    for (size_t i = 0; i < s_CurrentNameIDCount; i++)
    {
        if (s_NameIDList[i] == name)
        {
            return static_cast<int64_t>(i);
        }
    }

    if (s_CurrentNameIDCount < s_NameIDMaxCount)
    {
        s_NameIDList[s_CurrentNameIDCount] = name;
        s_CurrentNameIDCount++;
        return s_CurrentNameIDCount - 1;
    }

    SLUG_ASSERT_MSG(false, "Error NameID Count Limit...");
    return -1;
}

StringView GetNameIDStr(int64_t id)
{
    if (id >= 0 && id < s_NameIDMaxCount)
    {
        return s_NameIDList[id].data();
    }

    SLUG_ASSERT_MSG(false, "Error, Invaild NameID");
    return "___Failed_NameID";
}

NameID::NameID()
    : m_id(0)
{
    if (s_CurrentNameIDCount == 0)
    {
        s_NameIDList[s_CurrentNameIDCount] = "Default";
        s_CurrentNameIDCount++;
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

}
