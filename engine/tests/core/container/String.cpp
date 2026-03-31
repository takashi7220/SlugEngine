#include <gtest/gtest.h>
#include "core/container/String.hpp"

using namespace slug::core;

namespace
{

template <size_t N>
static String MakeStringOfLen(size_t len, char ch = 'a')
{
    return String(len, ch);
}

TEST(FixedStringTest, DefaultConstructor_Empty)
{
    FixedString<8> s;
    EXPECT_EQ(s.size(), 0u);
    EXPECT_EQ(s.capacity(), 7u);
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_EQ(s.data()[0], '\0');
}

TEST(FixedStringTest, ConstructFromCStr)
{
    FixedString<8> s("abc");
    EXPECT_EQ(s.size(), 3u);
    EXPECT_STREQ(s.c_str(), "abc");
    EXPECT_EQ(s.c_str()[3], '\0');
}

TEST(FixedStringTest, ConstructFromStringView)
{
    std::string_view sv = "hello";
    FixedString<16> s(sv);
    EXPECT_EQ(s.size(), 5u);
    EXPECT_STREQ(s.c_str(), "hello");
}

TEST(FixedStringTest, ConstructFromStdString)
{
    std::string str = "world";
    FixedString<16> s(str);
    EXPECT_EQ(s.size(), 5u);
    EXPECT_STREQ(s.c_str(), "world");
}

TEST(FixedStringTest, AssignFromCStr)
{
    FixedString<8> s;
    s = "xyz";
    EXPECT_EQ(s.size(), 3u);
    EXPECT_STREQ(s.c_str(), "xyz");
}

TEST(FixedStringTest, AssignFromStringView)
{
    FixedString<8> s;
    std::string_view sv = "1234";
    s = sv;
    EXPECT_EQ(s.size(), 4u);
    EXPECT_STREQ(s.c_str(), "1234");
}

TEST(FixedStringTest, AssignFromStdString)
{
    FixedString<8> s;
    std::string str = "k";
    s = str;
    EXPECT_EQ(s.size(), 1u);
    EXPECT_STREQ(s.c_str(), "k");
}

TEST(FixedStringTest, Capacity_IsNMinus1)
{
    FixedString<1> s1;
    EXPECT_EQ(s1.capacity(), 0u);

    FixedString<8> s8;
    EXPECT_EQ(s8.capacity(), 7u);
}

TEST(FixedStringTest, Assign_MaxLengthExactlyCapacity_OK)
{
    // N=8 なので最大長は 7
    constexpr size_t N = 8;
    FixedString<N> s;
    const String maxLen = MakeStringOfLen<N>(N - 1, 'm'); // len=7
    s = maxLen;
    EXPECT_EQ(s.size(), N - 1);
    EXPECT_EQ(StringView(s.c_str()), StringView(maxLen));
    EXPECT_EQ(s.c_str()[N - 1], '\0'); // 末尾 null 終端
}

TEST(FixedStringTest, Assign_LengthN_Throws)
{
    // N=8 のとき len=8 は sv.size() >= N で throw
    constexpr size_t N = 8;
    FixedString<N> s;
    const String tooLong = MakeStringOfLen<N>(N, 'x'); // len=8
    EXPECT_THROW(s = tooLong, std::exception);
}

TEST(FixedStringTest, Construct_LengthN_Throws)
{
    constexpr size_t N = 8;
    const String tooLong = MakeStringOfLen<N>(N, 'x'); // len=8
    EXPECT_THROW((FixedString<N>(tooLong)), std::exception);
}

TEST(FixedStringTest, OperatorIndex_ReadWrite_InRange)
{
    FixedString<8> s("abc");
    EXPECT_EQ(s[0], 'a');
    EXPECT_EQ(s[1], 'b');
    EXPECT_EQ(s[2], 'c');

    s[1] = 'Z';
    EXPECT_EQ(s[1], 'Z');
    EXPECT_STREQ(s.c_str(), "aZc");
}

TEST(FixedStringTest, OperatorIndex_IndexEqualsN_Throws)
{
    FixedString<8> s("abc");
    // 実装は if (i >= N) throw なので i==N で throw
    EXPECT_THROW((void)s[8], std::exception);
}

TEST(FixedStringTest, OperatorIndex_IndexGreaterThanN_Throws)
{
    FixedString<8> s("abc");
    EXPECT_THROW((void)s[999], std::exception);
}

TEST(FixedStringTest, View_ReturnsSameContentAndSize)
{
    FixedString<16> s("hello");
    auto v = s.view(); // core::StringView
    EXPECT_EQ(v.size(), 5u);
    // core::StringView が data()/size() を持つ前提
    EXPECT_EQ(std::string_view(v.data(), v.size()), std::string_view("hello", 5));
}

TEST(FixedStringTest, ToString_ReturnsSameContentAndSize)
{
    FixedString<16> s("hello");
    auto str = s.to_string(); // core::String
    // core::String が size() と data()/c_str() を持つ前提（適宜調整）
    EXPECT_EQ(str.size(), 5u);
    EXPECT_EQ(std::string_view(str.data(), str.size()), std::string_view("hello", 5));
}

TEST(FixedStringTest, CompareWithCoreStringView_EqualAndNotEqual)
{
    FixedString<16> s("abc");

    StringView same("abc", 3);
    StringView diff("abd", 3);

    EXPECT_TRUE(s == same);
    EXPECT_FALSE(s != same);

    EXPECT_FALSE(s == diff);
    EXPECT_TRUE(s != diff);
}

TEST(FixedStringTest, DataPointer_IsStableAndNullTerminated)
{
    FixedString<8> s("abc");
    const char* p1 = s.data();
    const char* p2 = s.c_str();
    EXPECT_EQ(p1, p2);
    EXPECT_EQ(p1[3], '\0');

    // 再代入しても同じバッファ（内部配列）を使う想定
    s = "xy";
    EXPECT_EQ(s.data(), p1);
    EXPECT_EQ(s.c_str()[2], '\0');
}

}
