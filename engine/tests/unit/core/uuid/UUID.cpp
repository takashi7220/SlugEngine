#include <gtest/gtest.h>
#include "core/uuid/UUID.hpp"

#include "core/container/UnorderedSet.hpp"

using namespace slug::core;

namespace
{

TEST(UUIDTest, CopiedUUIDComparesEqual)
{
    UUID uuid;
    UUID copy = uuid;

    EXPECT_TRUE(uuid == copy);
    EXPECT_EQ(uuid.GetHash(), copy.GetHash());
    EXPECT_EQ(uuid.Get(), copy.Get());
}

TEST(UUIDTest, GetReturnsSixteenByteView)
{
    UUID uuid;

    EXPECT_EQ(uuid.Get().size(), 16u);
}

TEST(UUIDTest, HashFunctorUsesUUIDHash)
{
    UUID uuid;
    UUIDHash hash;

    EXPECT_EQ(hash(uuid), uuid.GetHash());
}

TEST(UUIDTest, GeneratedUUIDsAreUsableAsUnorderedSetKeys)
{
    TUnorderedSet<UUID, UUIDHash> values;

    for (int i = 0; i < 32; ++i)
    {
        values.insert(UUID());
    }

    EXPECT_EQ(values.size(), 32u);
}

}
