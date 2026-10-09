#include <gtest/gtest.h>
#include "core/memory/UniquePtr.hpp"
#include "core/memory/MemoryObject.hpp"
#include "core/container/Vector.hpp"

using namespace slug::core;

namespace
{

class Tracked : public MemoryObject
{
public:
    Tracked()
        : value(0)
    {
        ++constructed;
    }

    explicit Tracked(int v)
        : value(v)
    {
        ++constructed;
    }

    Tracked(int a, int b)
        : value(a + b)
    {
        ++constructed;
    }

    ~Tracked() override
    {
        ++destructed;
    }

    static void ResetCounters()
    {
        constructed = 0;
        destructed = 0;
    }

    int value;
    static int constructed;
    static int destructed;
};

int Tracked::constructed = 0;
int Tracked::destructed = 0;

class Base : public MemoryObject
{
public:
    ~Base() override
    {
        ++destroyed;
    }

    static int destroyed;
};

class Derived : public Base
{
public:
    ~Derived() override
    {
        ++derivedDestroyed;
    }

    static int derivedDestroyed;
};

int Base::destroyed = 0;
int Derived::derivedDestroyed = 0;

} // namespace

//------------------------------------------------------------------------------
// 生成と単独所有
//------------------------------------------------------------------------------
TEST(TUniquePtrTest, DefaultConstructedIsNull)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TUniquePtr<Tracked> p;
        EXPECT_EQ(p.get(), nullptr);
        EXPECT_FALSE(static_cast<bool>(p));
    }
}

TEST(TUniquePtrTest, MakeUniqueDefaultConstructs)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        Tracked::ResetCounters();
        {
            TUniquePtr<Tracked> p = MakeUnique<Tracked>();
            ASSERT_NE(p.get(), nullptr);
            EXPECT_EQ(p->value, 0);
            EXPECT_EQ(Tracked::constructed, 1);
            EXPECT_EQ(Tracked::destructed, 0);
        }
        EXPECT_EQ(Tracked::destructed, 1);
    }
}

// MakeUnique は右辺値／左辺値のどちらも転送できる必要がある。
TEST(TUniquePtrTest, MakeUniqueForwardsRvalueArguments)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TUniquePtr<Tracked> p = MakeUnique<Tracked>(42);
        ASSERT_NE(p.get(), nullptr);
        EXPECT_EQ(p->value, 42);

        TUniquePtr<Tracked> q = MakeUnique<Tracked>(10, 20);
        ASSERT_NE(q.get(), nullptr);
        EXPECT_EQ(q->value, 30);
    }
}

TEST(TUniquePtrTest, MakeUniqueForwardsLvalueArguments)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        int a = 3;
        int b = 4;
        TUniquePtr<Tracked> p = MakeUnique<Tracked>(a, b);
        ASSERT_NE(p.get(), nullptr);
        EXPECT_EQ(p->value, 7);
    }
}

//------------------------------------------------------------------------------
// 所有権の移動
//------------------------------------------------------------------------------
TEST(TUniquePtrTest, MoveConstructionTransfersOwnership)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        Tracked::ResetCounters();
        {
            TUniquePtr<Tracked> src = MakeUnique<Tracked>(5);
            Tracked* raw = src.get();

            TUniquePtr<Tracked> dst(std::move(src));
            EXPECT_EQ(src.get(), nullptr);
            EXPECT_EQ(dst.get(), raw);
            EXPECT_EQ(Tracked::destructed, 0);
        }
        EXPECT_EQ(Tracked::destructed, 1);
    }
}

TEST(TUniquePtrTest, MoveAssignmentReleasesPreviousObject)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        Tracked::ResetCounters();
        {
            TUniquePtr<Tracked> a = MakeUnique<Tracked>(1);
            TUniquePtr<Tracked> b = MakeUnique<Tracked>(2);
            EXPECT_EQ(Tracked::constructed, 2);

            a = std::move(b);
            // 代入先が持っていたオブジェクトはここで破棄される。
            EXPECT_EQ(Tracked::destructed, 1);
            EXPECT_EQ(a->value, 2);
            EXPECT_EQ(b.get(), nullptr);
        }
        EXPECT_EQ(Tracked::destructed, 2);
    }
}

TEST(TUniquePtrTest, ResetDestroysObject)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        Tracked::ResetCounters();

        TUniquePtr<Tracked> p = MakeUnique<Tracked>(9);
        p.reset();
        EXPECT_EQ(p.get(), nullptr);
        EXPECT_EQ(Tracked::destructed, 1);

        // 解放済みに対する reset は無害。
        p.reset();
        EXPECT_EQ(Tracked::destructed, 1);
    }
}

TEST(TUniquePtrTest, ReleaseHandsOverRawPointer)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        Tracked::ResetCounters();

        TUniquePtr<Tracked> p = MakeUnique<Tracked>(11);
        Tracked* raw = p.release();
        EXPECT_EQ(p.get(), nullptr);
        ASSERT_NE(raw, nullptr);
        EXPECT_EQ(raw->value, 11);
        EXPECT_EQ(Tracked::destructed, 0);

        delete raw;
        EXPECT_EQ(Tracked::destructed, 1);
    }
}

TEST(TUniquePtrTest, SwapExchangesOwnership)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TUniquePtr<Tracked> a = MakeUnique<Tracked>(1);
        TUniquePtr<Tracked> b = MakeUnique<Tracked>(2);
        Tracked* rawA = a.get();
        Tracked* rawB = b.get();

        a.swap(b);
        EXPECT_EQ(a.get(), rawB);
        EXPECT_EQ(b.get(), rawA);
    }
}

//------------------------------------------------------------------------------
// 派生クラス・コンテナとの併用
//------------------------------------------------------------------------------
TEST(TUniquePtrTest, DestroysThroughBasePointer)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        Base::destroyed = 0;
        Derived::derivedDestroyed = 0;
        {
            TUniquePtr<Base> p = MakeUnique<Derived>();
            ASSERT_NE(p.get(), nullptr);
        }
        EXPECT_EQ(Derived::derivedDestroyed, 1);
        EXPECT_EQ(Base::destroyed, 1);
    }
}

TEST(TUniquePtrTest, UsableInsideVector)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        Tracked::ResetCounters();
        {
            TVector<TUniquePtr<Tracked>> items;
            for (int i = 0; i < 16; ++i)
            {
                items.push_back(MakeUnique<Tracked>(i));
            }

            ASSERT_EQ(items.size(), 16u);
            EXPECT_EQ(items[0]->value, 0);
            EXPECT_EQ(items[15]->value, 15);
            EXPECT_EQ(Tracked::destructed, 0);
        }
        EXPECT_EQ(Tracked::destructed, 16);
    }
}
