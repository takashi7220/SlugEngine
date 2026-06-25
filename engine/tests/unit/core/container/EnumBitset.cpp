#include <gtest/gtest.h>
#include "core/container/EnumBitset.hpp"

using namespace slug::core;

namespace
{

enum class TestFlag
{
    Read,
    Write,
    Execute,
    Count
};

TEST(EnumBitsetTest, StartsEmpty)
{
    TEnumBitset<TestFlag> flags;

    EXPECT_FALSE(flags.Any());
    EXPECT_FALSE(flags.Test(TestFlag::Read));
    EXPECT_FALSE(flags.Test(TestFlag::Write));
    EXPECT_FALSE(flags.Test(TestFlag::Execute));
}

TEST(EnumBitsetTest, SetAndResetIndividualFlags)
{
    TEnumBitset<TestFlag> flags;

    flags.Set(TestFlag::Read);
    flags.Set(TestFlag::Execute);

    EXPECT_TRUE(flags.Any());
    EXPECT_TRUE(flags.Test(TestFlag::Read));
    EXPECT_FALSE(flags.Test(TestFlag::Write));
    EXPECT_TRUE(flags.Test(TestFlag::Execute));

    flags.Reset(TestFlag::Read);

    EXPECT_FALSE(flags.Test(TestFlag::Read));
    EXPECT_TRUE(flags.Test(TestFlag::Execute));
}

TEST(EnumBitsetTest, AssignmentSetsFlagWithoutClearingExistingBits)
{
    TEnumBitset<TestFlag> flags(TestFlag::Read);

    flags = TestFlag::Write;

    EXPECT_TRUE(flags.Test(TestFlag::Read));
    EXPECT_TRUE(flags.Test(TestFlag::Write));
    EXPECT_FALSE(flags.Test(TestFlag::Execute));
}

TEST(EnumBitsetTest, BitwiseOperatorsReturnExpectedIntersectionAndUnion)
{
    TEnumBitset<TestFlag> readWrite;
    readWrite.Set(TestFlag::Read);
    readWrite.Set(TestFlag::Write);

    TEnumBitset<TestFlag> writeExecute;
    writeExecute.Set(TestFlag::Write);
    writeExecute.Set(TestFlag::Execute);

    auto intersection = readWrite & writeExecute;
    auto unionFlags = readWrite | writeExecute;

    EXPECT_FALSE(intersection.Test(TestFlag::Read));
    EXPECT_TRUE(intersection.Test(TestFlag::Write));
    EXPECT_FALSE(intersection.Test(TestFlag::Execute));

    EXPECT_TRUE(unionFlags.Test(TestFlag::Read));
    EXPECT_TRUE(unionFlags.Test(TestFlag::Write));
    EXPECT_TRUE(unionFlags.Test(TestFlag::Execute));
}

TEST(EnumBitSetUtilityTest, CheckSetClearAndToggleBits)
{
    uint32_t value = 0;

    EnumBitSetUtility::Set(value, 0x01);
    EnumBitSetUtility::Set(value, 0x04);

    EXPECT_TRUE(EnumBitSetUtility::Check(value, 0x01));
    EXPECT_TRUE(EnumBitSetUtility::Check(value, 0x04));
    EXPECT_FALSE(EnumBitSetUtility::Equal(value, 0x01));

    EnumBitSetUtility::Clear(value, 0x01);
    EXPECT_FALSE(EnumBitSetUtility::Check(value, 0x01));
    EXPECT_TRUE(EnumBitSetUtility::Equal(value, 0x04));

    EnumBitSetUtility::Toggle(value, 0x04);
    EXPECT_EQ(value, 0u);
}

}
