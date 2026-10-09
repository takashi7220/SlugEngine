#include <gtest/gtest.h>
#include "core/container/StaticVector.hpp"
#include "core/container/String.hpp"

using namespace slug::core;

namespace
{
using IntVector8 = TStaticVector<int, 8>;
}

//------------------------------------------------------------------------------
// 構築
//------------------------------------------------------------------------------
TEST(TStaticVectorTest, DefaultConstructedIsEmpty)
{
    IntVector8 v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.max_size(), 8u);
    EXPECT_EQ(IntVector8::MAX_ELEMENT, 8u);
    EXPECT_EQ(v.begin(), v.end());
}

TEST(TStaticVectorTest, SizeConstructorSetsCurrentSize)
{
    IntVector8 v(3);
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(std::distance(v.begin(), v.end()), 3);
}

TEST(TStaticVectorTest, InitializerListFillsInOrder)
{
    IntVector8 v { 10, 20, 30 };

    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);
    EXPECT_EQ(v.front(), 10);
    EXPECT_EQ(v.back(), 30);
}

//------------------------------------------------------------------------------
// 追加・削除
//------------------------------------------------------------------------------
TEST(TStaticVectorTest, PushBackAndPopBack)
{
    IntVector8 v;

    for (int i = 0; i < 8; ++i)
    {
        v.push_back(i);
        EXPECT_EQ(v.size(), static_cast<size_t>(i + 1));
        EXPECT_EQ(v.back(), i);
    }

    EXPECT_EQ(v.size(), v.max_size());

    for (int i = 7; i >= 0; --i)
    {
        EXPECT_EQ(v.back(), i);
        v.pop_back();
        EXPECT_EQ(v.size(), static_cast<size_t>(i));
    }
    EXPECT_TRUE(v.empty());
}

TEST(TStaticVectorTest, PushBackAcceptsRvalue)
{
    TStaticVector<String, 4> v;
    String moved = "moved-value";

    v.push_back(std::move(moved));

    ASSERT_EQ(v.size(), 1u);
    EXPECT_EQ(v[0], "moved-value");
}

TEST(TStaticVectorTest, ResizeGrowsAndShrinks)
{
    IntVector8 v { 1, 2, 3 };

    v.resize(6);
    EXPECT_EQ(v.size(), 6u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[2], 3);

    v.resize(2);
    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[1], 2);

    v.resize(0);
    EXPECT_TRUE(v.empty());
}

TEST(TStaticVectorTest, EmplaceBackReturnsNewElement)
{
    IntVector8 v;

    int& first = v.emplace_back();
    EXPECT_EQ(v.size(), 1u);
    first = 99;
    EXPECT_EQ(v[0], 99);
    EXPECT_EQ(v.back(), 99);

    v.emplace_back() = 100;
    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[1], 100);
}

TEST(TStaticVectorTest, FillSetsSizeToCapacity)
{
    IntVector8 v { 1, 2 };

    v.fill(7);

    EXPECT_EQ(v.size(), v.max_size());
    for (size_t i = 0; i < v.size(); ++i)
    {
        EXPECT_EQ(v[i], 7);
    }
}

//------------------------------------------------------------------------------
// 走査
//------------------------------------------------------------------------------
TEST(TStaticVectorTest, IterationVisitsOnlyStoredElements)
{
    IntVector8 v { 1, 2, 3 };

    int count = 0;
    int sum = 0;
    for (int value : v)
    {
        ++count;
        sum += value;
    }

    EXPECT_EQ(count, 3);
    EXPECT_EQ(sum, 6);
}

// end() const が容量末尾を返していると、const 参照越しの範囲 for が
// 未使用領域まで走査してしまう。
TEST(TStaticVectorTest, ConstIterationVisitsOnlyStoredElements)
{
    IntVector8 v { 4, 5 };
    const IntVector8& constRef = v;

    EXPECT_EQ(std::distance(constRef.begin(), constRef.end()), 2);
    EXPECT_EQ(std::distance(constRef.cbegin(), constRef.cend()), 2);

    int count = 0;
    for (int value : constRef)
    {
        (void)value;
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST(TStaticVectorTest, ConstAccessorsReadStoredElements)
{
    IntVector8 v { 11, 22, 33 };
    const IntVector8& constRef = v;

    EXPECT_EQ(constRef[0], 11);
    EXPECT_EQ(constRef.front(), 11);
    EXPECT_EQ(constRef.back(), 33);
    EXPECT_EQ(constRef.size(), 3u);
    EXPECT_FALSE(constRef.empty());
}

//------------------------------------------------------------------------------
// コピー・swap
//------------------------------------------------------------------------------
TEST(TStaticVectorTest, CopyPreservesContentAndSize)
{
    IntVector8 src { 1, 2, 3 };
    IntVector8 copy(src);

    ASSERT_EQ(copy.size(), src.size());
    for (size_t i = 0; i < src.size(); ++i)
    {
        EXPECT_EQ(copy[i], src[i]);
    }

    IntVector8 assigned;
    assigned = src;
    ASSERT_EQ(assigned.size(), 3u);
    EXPECT_EQ(assigned[2], 3);

    // コピーは独立していること。
    copy[0] = 100;
    EXPECT_EQ(src[0], 1);
}

// swap が自分自身を渡していると要素が入れ替わらず、サイズだけが交換される。
TEST(TStaticVectorTest, SwapExchangesContentAndSize)
{
    IntVector8 a { 1, 2, 3 };
    IntVector8 b { 7, 8 };

    a.swap(b);

    ASSERT_EQ(a.size(), 2u);
    EXPECT_EQ(a[0], 7);
    EXPECT_EQ(a[1], 8);

    ASSERT_EQ(b.size(), 3u);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);
}

TEST(TStaticVectorTest, DataPointsToContiguousStorage)
{
    IntVector8 v { 1, 2, 3 };

    const int* data = v.data();
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data[0], 1);
    EXPECT_EQ(data[1], 2);
    EXPECT_EQ(data[2], 3);
}
