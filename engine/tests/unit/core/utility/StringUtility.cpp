#include <gtest/gtest.h>
#include "core/container/String.hpp"
#include "core/utility/Hash.hpp"

using namespace slug::core;

namespace
{

TEST(StringUtilityTest, SplitReturnsAllTokens)
{
    auto tokens = StringUtility::Split("alpha,beta,gamma", ",");

    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0], "alpha");
    EXPECT_EQ(tokens[1], "beta");
    EXPECT_EQ(tokens[2], "gamma");
}

TEST(StringUtilityTest, SplitPreservesEmptyTokens)
{
    auto tokens = StringUtility::Split("alpha,,gamma,", ",");

    ASSERT_EQ(tokens.size(), 4u);
    EXPECT_EQ(tokens[0], "alpha");
    EXPECT_EQ(tokens[1], "");
    EXPECT_EQ(tokens[2], "gamma");
    EXPECT_EQ(tokens[3], "");
}

TEST(StringUtilityTest, SnprintfAppendsFormattedText)
{
    String text = "value=";

    StringUtility::Snprintf(text, "%d:%s", 42, "ok");

    EXPECT_EQ(text, "value=42:ok");
}

TEST(StringUtilityTest, ToStringConvertsPrimitiveValues)
{
    EXPECT_EQ(StringUtility::ToString(static_cast<int32_t>(-12)), "-12");
    EXPECT_EQ(StringUtility::ToString(static_cast<uint32_t>(12)), "12");
    EXPECT_EQ(StringUtility::ToString(1.5f), "1.500000");
}

TEST(HashUtilityTest, ConvertStringToHash64UsesFnv1A)
{
    EXPECT_EQ(HashUtility::ConvertStringToHash64(""), 14695981039346656037ull);
    EXPECT_EQ(HashUtility::ConvertStringToHash64("slug"), 7696373693962291038ull);
}

TEST(HashUtilityTest, HashCombineChangesSeedAndIsDeterministic)
{
    size_t first = 0;
    size_t second = 0;

    HashUtility::HashCombine(first, 123u);
    HashUtility::HashCombine(second, 123u);

    EXPECT_NE(first, 0u);
    EXPECT_EQ(first, second);
}

}
