#include <gtest/gtest.h>
#include "core/container/IntrusiveLinkedList.hpp"
#include "core/container/Vector.hpp"

using namespace slug::core;

namespace
{

struct Payload
{
    int value = 0;
};

using List = IntrusiveLinkedList<Payload>;

// ノードと実体をまとめて持つテスト用ヘルパ。
struct Element
{
    explicit Element(int v)
    {
        payload.value = v;
        node.data = &payload;
    }

    Payload payload;
    List::Node node;
};

TVector<int> Collect(List& list)
{
    TVector<int> values;
    list.for_each([&values](Payload* p)
    {
        values.push_back(p ? p->value : -1);
    });
    return values;
}

TVector<int> CollectReverse(List& list)
{
    TVector<int> values;
    list.for_each_reverse([&values](Payload* p)
    {
        values.push_back(p ? p->value : -1);
    });
    return values;
}

} // namespace

//------------------------------------------------------------------------------
// 基本
//------------------------------------------------------------------------------
TEST(IntrusiveLinkedListTest, EmptyListVisitsNothing)
{
    List list;

    EXPECT_TRUE(Collect(list).empty());
    EXPECT_TRUE(CollectReverse(list).empty());
}

TEST(IntrusiveLinkedListTest, PushBackKeepsInsertionOrder)
{
    List list;
    Element a(1);
    Element b(2);
    Element c(3);

    list.push_back(&a.node);
    list.push_back(&b.node);
    list.push_back(&c.node);

    const TVector<int> forward = Collect(list);
    ASSERT_EQ(forward.size(), 3u);
    EXPECT_EQ(forward[0], 1);
    EXPECT_EQ(forward[1], 2);
    EXPECT_EQ(forward[2], 3);
}

TEST(IntrusiveLinkedListTest, ForEachReverseWalksFromTail)
{
    List list;
    Element a(1);
    Element b(2);
    Element c(3);

    list.push_back(&a.node);
    list.push_back(&b.node);
    list.push_back(&c.node);

    const TVector<int> reverse = CollectReverse(list);
    ASSERT_EQ(reverse.size(), 3u);
    EXPECT_EQ(reverse[0], 3);
    EXPECT_EQ(reverse[1], 2);
    EXPECT_EQ(reverse[2], 1);
}

TEST(IntrusiveLinkedListTest, SingleNodeListIsWalkableBothWays)
{
    List list;
    Element a(42);

    list.push_back(&a.node);

    const TVector<int> forward = Collect(list);
    const TVector<int> reverse = CollectReverse(list);
    ASSERT_EQ(forward.size(), 1u);
    ASSERT_EQ(reverse.size(), 1u);
    EXPECT_EQ(forward[0], 42);
    EXPECT_EQ(reverse[0], 42);
}

//------------------------------------------------------------------------------
// remove
//------------------------------------------------------------------------------
TEST(IntrusiveLinkedListTest, RemoveHeadUpdatesHead)
{
    List list;
    Element a(1);
    Element b(2);
    Element c(3);
    list.push_back(&a.node);
    list.push_back(&b.node);
    list.push_back(&c.node);

    list.remove(&a.node);

    const TVector<int> forward = Collect(list);
    ASSERT_EQ(forward.size(), 2u);
    EXPECT_EQ(forward[0], 2);
    EXPECT_EQ(forward[1], 3);

    // 逆方向から辿っても tail/prev が壊れていないこと。
    const TVector<int> reverse = CollectReverse(list);
    ASSERT_EQ(reverse.size(), 2u);
    EXPECT_EQ(reverse[0], 3);
    EXPECT_EQ(reverse[1], 2);

    // 外したノードのリンクはクリアされる。
    EXPECT_EQ(a.node.next, nullptr);
    EXPECT_EQ(a.node.prev, nullptr);
}

TEST(IntrusiveLinkedListTest, RemoveMiddleKeepsBothSidesLinked)
{
    List list;
    Element a(1);
    Element b(2);
    Element c(3);
    list.push_back(&a.node);
    list.push_back(&b.node);
    list.push_back(&c.node);

    list.remove(&b.node);

    const TVector<int> forward = Collect(list);
    ASSERT_EQ(forward.size(), 2u);
    EXPECT_EQ(forward[0], 1);
    EXPECT_EQ(forward[1], 3);

    const TVector<int> reverse = CollectReverse(list);
    ASSERT_EQ(reverse.size(), 2u);
    EXPECT_EQ(reverse[0], 3);
    EXPECT_EQ(reverse[1], 1);
}

TEST(IntrusiveLinkedListTest, RemoveTailUpdatesTail)
{
    List list;
    Element a(1);
    Element b(2);
    Element c(3);
    list.push_back(&a.node);
    list.push_back(&b.node);
    list.push_back(&c.node);

    list.remove(&c.node);

    const TVector<int> reverse = CollectReverse(list);
    ASSERT_EQ(reverse.size(), 2u);
    EXPECT_EQ(reverse[0], 2);
    EXPECT_EQ(reverse[1], 1);
}

TEST(IntrusiveLinkedListTest, RemoveOnlyNodeEmptiesList)
{
    List list;
    Element a(1);
    list.push_back(&a.node);

    list.remove(&a.node);

    EXPECT_TRUE(Collect(list).empty());
    EXPECT_TRUE(CollectReverse(list).empty());
}

TEST(IntrusiveLinkedListTest, RemoveAllNodesOneByOne)
{
    List list;
    Element a(1);
    Element b(2);
    Element c(3);
    list.push_back(&a.node);
    list.push_back(&b.node);
    list.push_back(&c.node);

    list.remove(&b.node);
    list.remove(&c.node);
    list.remove(&a.node);

    EXPECT_TRUE(Collect(list).empty());
    EXPECT_TRUE(CollectReverse(list).empty());
}

TEST(IntrusiveLinkedListTest, NodeCanBeReinsertedAfterRemove)
{
    List list;
    Element a(1);
    Element b(2);
    list.push_back(&a.node);
    list.push_back(&b.node);

    list.remove(&a.node);
    list.push_back(&a.node);

    const TVector<int> forward = Collect(list);
    ASSERT_EQ(forward.size(), 2u);
    EXPECT_EQ(forward[0], 2);
    EXPECT_EQ(forward[1], 1);
}

//------------------------------------------------------------------------------
// clear
//------------------------------------------------------------------------------
TEST(IntrusiveLinkedListTest, ClearEmptiesListAndResetsNodeLinks)
{
    List list;
    Element a(1);
    Element b(2);
    Element c(3);
    list.push_back(&a.node);
    list.push_back(&b.node);
    list.push_back(&c.node);

    list.clear();

    EXPECT_TRUE(Collect(list).empty());
    EXPECT_TRUE(CollectReverse(list).empty());

    EXPECT_EQ(a.node.next, nullptr);
    EXPECT_EQ(a.node.prev, nullptr);
    EXPECT_EQ(b.node.next, nullptr);
    EXPECT_EQ(b.node.prev, nullptr);
    EXPECT_EQ(c.node.next, nullptr);
    EXPECT_EQ(c.node.prev, nullptr);
}

TEST(IntrusiveLinkedListTest, ClearOnEmptyListIsNoop)
{
    List list;
    list.clear();
    EXPECT_TRUE(Collect(list).empty());
}

TEST(IntrusiveLinkedListTest, ReusableAfterClear)
{
    List list;
    Element a(1);
    Element b(2);
    list.push_back(&a.node);
    list.push_back(&b.node);
    list.clear();

    list.push_back(&b.node);
    list.push_back(&a.node);

    const TVector<int> forward = Collect(list);
    ASSERT_EQ(forward.size(), 2u);
    EXPECT_EQ(forward[0], 2);
    EXPECT_EQ(forward[1], 1);
}

//------------------------------------------------------------------------------
// for_each でのデータ書き換え
//------------------------------------------------------------------------------
TEST(IntrusiveLinkedListTest, ForEachCanMutatePayload)
{
    List list;
    Element a(1);
    Element b(2);
    list.push_back(&a.node);
    list.push_back(&b.node);

    list.for_each([](Payload* p)
    {
        p->value *= 10;
    });

    EXPECT_EQ(a.payload.value, 10);
    EXPECT_EQ(b.payload.value, 20);
}
