#include <gtest/gtest.h>
#include "core/uuid/NameID.hpp"

using namespace slug::core;

namespace
{

TEST(NameIDTest, SameNameSharesIdentity)
{
    NameID first("UnitTest.NameID.SameName");
    NameID second("UnitTest.NameID.SameName");

    EXPECT_TRUE(first == second);
    EXPECT_EQ(first.GetStr(), "UnitTest.NameID.SameName");
    EXPECT_EQ(second.GetStr(), "UnitTest.NameID.SameName");
    EXPECT_EQ(first.GetHash(), second.GetHash());
}

TEST(NameIDTest, DifferentNamesHaveDifferentIdentity)
{
    NameID first("UnitTest.NameID.First");
    NameID second("UnitTest.NameID.Second");

    EXPECT_FALSE(first == second);
    EXPECT_NE(first.GetStr(), second.GetStr());
    EXPECT_NE(first.GetHash(), second.GetHash());
}

TEST(NameIDTest, HashFunctorUsesStoredIdentity)
{
    NameID first("UnitTest.NameID.Hash");
    NameID second("UnitTest.NameID.Hash");
    NameIDHash hash;

    EXPECT_EQ(hash(first), first.GetHash());
    EXPECT_EQ(hash(first), hash(second));
}

}
