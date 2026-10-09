#include <gtest/gtest.h>
#include "core/pattern/ResourcePool.hpp"
#include "core/pattern/Singleton.hpp"
#include "core/pattern/Closure.hpp"
#include "core/container/String.hpp"
#include "core/thread/Thread.hpp"

#include <memory>

using namespace slug::core;

namespace
{

using IntPool = ResourcePool<int, String>;

class Counter
{
public:
    int Add(int v)
    {
        total += v;
        return total;
    }

    int Get() const
    {
        return total;
    }

    void Reset()
    {
        total = 0;
    }

    int total = 0;
};

class SingletonTarget : public TSingleton<SingletonTarget>
{
public:
    SingletonTarget()
    {
        ++liveCount;
    }

    ~SingletonTarget() override
    {
        --liveCount;
    }

    int value = 0;
    static int liveCount;
};

int SingletonTarget::liveCount = 0;

} // namespace

//------------------------------------------------------------------------------
// ResourcePool
//------------------------------------------------------------------------------
TEST(ResourcePoolTest, GetOnEmptyPoolFails)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        IntPool pool;
        String value = "untouched";

        EXPECT_FALSE(pool.Contain(1));
        EXPECT_FALSE(pool.Get(1, value));
        EXPECT_EQ(value, "untouched");
    }
}

TEST(ResourcePoolTest, AddThenGetReturnsStoredValue)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        IntPool pool;
        pool.Add(1, "one");
        pool.Add(2, "two");

        EXPECT_TRUE(pool.Contain(1));
        EXPECT_TRUE(pool.Contain(2));
        EXPECT_FALSE(pool.Contain(3));

        String value;
        ASSERT_TRUE(pool.Get(1, value));
        EXPECT_EQ(value, "one");
        ASSERT_TRUE(pool.Get(2, value));
        EXPECT_EQ(value, "two");
    }
}

// Add は既存キーを上書きしない契約。
TEST(ResourcePoolTest, AddDoesNotOverwriteExistingKey)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        IntPool pool;
        pool.Add(1, "first");
        pool.Add(1, "second");

        String value;
        ASSERT_TRUE(pool.Get(1, value));
        EXPECT_EQ(value, "first");
    }
}

TEST(ResourcePoolTest, DeleteRemovesEntry)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        IntPool pool;
        pool.Add(1, "one");
        pool.Add(2, "two");

        pool.Delete(1);

        EXPECT_FALSE(pool.Contain(1));
        EXPECT_TRUE(pool.Contain(2));

        String value;
        EXPECT_FALSE(pool.Get(1, value));
    }
}

TEST(ResourcePoolTest, DeleteMissingKeyIsNoop)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        IntPool pool;
        pool.Add(1, "one");

        pool.Delete(99);

        EXPECT_TRUE(pool.Contain(1));
    }
}

TEST(ResourcePoolTest, ReAddAfterDelete)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        IntPool pool;
        pool.Add(1, "first");
        pool.Delete(1);
        pool.Add(1, "second");

        String value;
        ASSERT_TRUE(pool.Get(1, value));
        EXPECT_EQ(value, "second");
    }
}

// 各公開メソッドが内部ロックを再取得して自己デッドロックしないことを確認する。
TEST(ResourcePoolTest, ConcurrentAccessDoesNotDeadlock)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        IntPool pool;
        constexpr int threadCount = 4;
        constexpr int iterations = 200;

        TVector<Thread> threads;
        threads.reserve(threadCount);
        for (int t = 0; t < threadCount; ++t)
        {
            threads.emplace_back([&pool, t]()
            {
                for (int i = 0; i < iterations; ++i)
                {
                    const int key = (t * iterations) + i;
                    pool.Add(key, "value");

                    String value;
                    pool.Get(key, value);
                    pool.Contain(key);
                    pool.Delete(key);
                }
            });
        }

        for (auto& th : threads)
        {
            th.join();
        }

        EXPECT_FALSE(pool.Contain(0));
    }
}

//------------------------------------------------------------------------------
// TSingleton
//------------------------------------------------------------------------------
TEST(TSingletonTest, GetCreatesInstanceLazily)
{
    SingletonTarget::Destroy();
    EXPECT_EQ(SingletonTarget::liveCount, 0);

    SingletonTarget& first = SingletonTarget::Get();
    EXPECT_EQ(SingletonTarget::liveCount, 1);

    SingletonTarget& second = SingletonTarget::Get();
    EXPECT_EQ(&first, &second);
    EXPECT_EQ(SingletonTarget::liveCount, 1);

    SingletonTarget::Destroy();
    EXPECT_EQ(SingletonTarget::liveCount, 0);
}

TEST(TSingletonTest, CreateIsIdempotent)
{
    SingletonTarget::Destroy();

    SingletonTarget::Create();
    SingletonTarget* first = &SingletonTarget::Get();
    SingletonTarget::Create();
    SingletonTarget* second = &SingletonTarget::Get();

    EXPECT_EQ(first, second);
    EXPECT_EQ(SingletonTarget::liveCount, 1);

    SingletonTarget::Destroy();
}

TEST(TSingletonTest, DestroyIsIdempotentAndRecreates)
{
    SingletonTarget::Destroy();
    SingletonTarget::Destroy();
    EXPECT_EQ(SingletonTarget::liveCount, 0);

    SingletonTarget::Get().value = 5;
    EXPECT_EQ(SingletonTarget::Get().value, 5);

    SingletonTarget::Destroy();
    // 破棄後に取得すると新しい実体が作られ、状態は初期値に戻る。
    EXPECT_EQ(SingletonTarget::Get().value, 0);

    SingletonTarget::Destroy();
}

//------------------------------------------------------------------------------
// ClosureUtility
//------------------------------------------------------------------------------
TEST(ClosureUtilityTest, MakeClosureFromReferenceBindsInstance)
{
    Counter counter;

    auto add = ClosureUtility::MakeClosure(counter, &Counter::Add);

    EXPECT_EQ(add(3), 3);
    EXPECT_EQ(add(4), 7);
    EXPECT_EQ(counter.Get(), 7);
}

TEST(ClosureUtilityTest, MakeClosureFromPointerBindsInstance)
{
    Counter counter;

    auto add = ClosureUtility::MakeClosure(&counter, &Counter::Add);

    EXPECT_EQ(add(10), 10);
    EXPECT_EQ(counter.Get(), 10);
}

TEST(ClosureUtilityTest, MakeClosureFromConstReferenceCallsConstMethod)
{
    Counter counter;
    counter.Add(12);
    const Counter& constRef = counter;

    auto get = ClosureUtility::MakeClosure(constRef, &Counter::Get);

    EXPECT_EQ(get(), 12);
}

TEST(ClosureUtilityTest, MakeClosureFromConstPointerCallsConstMethod)
{
    Counter counter;
    counter.Add(21);
    const Counter* constPtr = &counter;

    auto get = ClosureUtility::MakeClosure(constPtr, &Counter::Get);

    EXPECT_EQ(get(), 21);
}

TEST(ClosureUtilityTest, MakeClosureReflectsLaterStateChanges)
{
    Counter counter;
    auto get = ClosureUtility::MakeClosure(&counter, &Counter::Get);

    EXPECT_EQ(get(), 0);
    counter.Add(5);
    EXPECT_EQ(get(), 5);
}

TEST(ClosureUtilityTest, MakeWeakClosureCallsWhileAlive)
{
    auto owner = std::make_shared<Counter>();

    auto add = ClosureUtility::MakeWeakClosure(owner, &Counter::Add);

    EXPECT_EQ(add(6), 6);
    EXPECT_EQ(owner->Get(), 6);
}

TEST(ClosureUtilityTest, MakeWeakClosureThrowsAfterOwnerReleased)
{
    auto owner = std::make_shared<Counter>();
    auto add = ClosureUtility::MakeWeakClosure(owner, &Counter::Add);

    owner.reset();

    EXPECT_THROW(add(1), std::bad_function_call);
}
