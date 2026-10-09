#include <gtest/gtest.h>
#include "core/task/WorkQueue.hpp"
#include "core/container/Vector.hpp"
#include "core/thread/Thread.hpp"

#include <chrono>

using namespace slug::core;

namespace
{

TReferencePtr<Task> MakeTask(TaskPriority priority)
{
    return MakeReference<Task>(priority);
}

TaskPriority PopPriority(WorkQueue& queue)
{
    TReferencePtr<Task> task;
    EXPECT_TRUE(queue.TryPop(task));
    EXPECT_NE(task.get(), nullptr);
    return task->GetPriority();
}

} // namespace

//------------------------------------------------------------------------------
// 基本
//------------------------------------------------------------------------------
TEST(WorkQueueTest, EmptyQueueTryPopFails)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;
        TReferencePtr<Task> task;

        EXPECT_EQ(queue.Size(), 0u);
        EXPECT_FALSE(queue.TryPop(task));
        EXPECT_EQ(task.get(), nullptr);
    }
}

TEST(WorkQueueTest, SizeCountsAllPriorityLevels)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;
        queue.Push(MakeTask(TaskPriority::Critical));
        queue.Push(MakeTask(TaskPriority::Normal));
        queue.Push(MakeTask(TaskPriority::Background));

        EXPECT_EQ(queue.Size(), 3u);

        TReferencePtr<Task> task;
        ASSERT_TRUE(queue.TryPop(task));
        EXPECT_EQ(queue.Size(), 2u);
    }
}

TEST(WorkQueueTest, PushPopPreservesTaskIdentity)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;
        TReferencePtr<Task> pushed = MakeTask(TaskPriority::Normal);
        Task* raw = pushed.get();

        queue.Push(pushed);

        TReferencePtr<Task> popped;
        ASSERT_TRUE(queue.TryPop(popped));
        EXPECT_EQ(popped.get(), raw);
    }
}

//------------------------------------------------------------------------------
// 優先度順
//------------------------------------------------------------------------------
TEST(WorkQueueTest, PopsInPriorityOrder)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;

        // 低い優先度から積んでも、高い優先度から取り出される。
        queue.Push(MakeTask(TaskPriority::Background));
        queue.Push(MakeTask(TaskPriority::Low));
        queue.Push(MakeTask(TaskPriority::Normal));
        queue.Push(MakeTask(TaskPriority::High));
        queue.Push(MakeTask(TaskPriority::Critical));

        EXPECT_EQ(PopPriority(queue), TaskPriority::Critical);
        EXPECT_EQ(PopPriority(queue), TaskPriority::High);
        EXPECT_EQ(PopPriority(queue), TaskPriority::Normal);
        EXPECT_EQ(PopPriority(queue), TaskPriority::Low);
        EXPECT_EQ(PopPriority(queue), TaskPriority::Background);
    }
}

TEST(WorkQueueTest, SamePriorityIsFirstInFirstOut)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;

        TVector<Task*> pushedOrder;
        for (int i = 0; i < 4; ++i)
        {
            TReferencePtr<Task> task = MakeTask(TaskPriority::Normal);
            pushedOrder.push_back(task.get());
            queue.Push(task);
        }

        for (size_t i = 0; i < pushedOrder.size(); ++i)
        {
            TReferencePtr<Task> task;
            ASSERT_TRUE(queue.TryPop(task));
            EXPECT_EQ(task.get(), pushedOrder[i]) << "index=" << i;
        }
    }
}

//------------------------------------------------------------------------------
// 飢餓クォータ
//------------------------------------------------------------------------------
// 低優先度がクォータ分スキップされたら、優先度を無視して割り込む。
TEST(WorkQueueTest, StarvationQuotaPromotesSkippedLevel)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;

        queue.Push(MakeTask(TaskPriority::Low));
        for (uint32_t i = 0; i < TaskQueueStarvationQuota + 4; ++i)
        {
            queue.Push(MakeTask(TaskPriority::Critical));
        }

        // クォータに達するまでは Critical が優先される。
        for (uint32_t i = 0; i < TaskQueueStarvationQuota; ++i)
        {
            EXPECT_EQ(PopPriority(queue), TaskPriority::Critical) << "index=" << i;
        }

        // ちょうどクォータ回スキップされた直後に Low が割り込む。
        EXPECT_EQ(PopPriority(queue), TaskPriority::Low);

        // 以降は再び Critical に戻る。
        EXPECT_EQ(PopPriority(queue), TaskPriority::Critical);
    }
}

TEST(WorkQueueTest, SkipCountOnlyAdvancesWhileLevelHasWork)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;

        // Low が空のまま Critical を流してもスキップは加算されない。
        for (uint32_t i = 0; i < TaskQueueStarvationQuota * 2; ++i)
        {
            queue.Push(MakeTask(TaskPriority::Critical));
            EXPECT_EQ(PopPriority(queue), TaskPriority::Critical);
        }

        // ここで初めて Low を積むので、割り込みにはクォータ分のスキップが必要。
        queue.Push(MakeTask(TaskPriority::Low));
        for (uint32_t i = 0; i < TaskQueueStarvationQuota; ++i)
        {
            queue.Push(MakeTask(TaskPriority::Critical));
        }

        for (uint32_t i = 0; i < TaskQueueStarvationQuota; ++i)
        {
            EXPECT_EQ(PopPriority(queue), TaskPriority::Critical) << "index=" << i;
        }
        EXPECT_EQ(PopPriority(queue), TaskPriority::Low);
    }
}

//------------------------------------------------------------------------------
// PopBlocking
//------------------------------------------------------------------------------
TEST(WorkQueueTest, PopBlockingReturnsQueuedTaskImmediately)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;
        TAtomic<bool> stopFlag { false };

        queue.Push(MakeTask(TaskPriority::Normal));

        TReferencePtr<Task> task;
        ASSERT_TRUE(queue.PopBlocking(task, stopFlag));
        EXPECT_NE(task.get(), nullptr);
    }
}

TEST(WorkQueueTest, PopBlockingFailsWhenStoppedAndEmpty)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;
        TAtomic<bool> stopFlag { true };

        TReferencePtr<Task> task;
        EXPECT_FALSE(queue.PopBlocking(task, stopFlag));
        EXPECT_EQ(task.get(), nullptr);
    }
}

// 停止済みでも残っているタスクは取り出せる。
TEST(WorkQueueTest, PopBlockingDrainsRemainingTasksAfterStop)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;
        TAtomic<bool> stopFlag { false };

        queue.Push(MakeTask(TaskPriority::Normal));
        queue.Push(MakeTask(TaskPriority::Normal));
        stopFlag.store(true);

        TReferencePtr<Task> task;
        EXPECT_TRUE(queue.PopBlocking(task, stopFlag));
        EXPECT_TRUE(queue.PopBlocking(task, stopFlag));
        EXPECT_FALSE(queue.PopBlocking(task, stopFlag));
    }
}

TEST(WorkQueueTest, PopBlockingWakesOnPush)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        WorkQueue queue;
        TAtomic<bool> stopFlag { false };
        TAtomic<bool> popped { false };

        Thread waiter([&queue, &stopFlag, &popped]()
        {
            TReferencePtr<Task> task;
            if (queue.PopBlocking(task, stopFlag))
            {
                popped.store(true);
            }
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        queue.Push(MakeTask(TaskPriority::Normal));

        waiter.join();
        EXPECT_TRUE(popped.load());
    }
}

// NotifyAll が待機側のミューテックスと同期しないと通知が消失し、
// ここで待機スレッドが起床できなくなる。
TEST(WorkQueueTest, NotifyAllWakesBlockedWaiterAfterStopFlagIsSet)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        for (int attempt = 0; attempt < 200; ++attempt)
        {
            WorkQueue queue;
            TAtomic<bool> stopFlag { false };
            TAtomic<bool> finished { false };

            Thread waiter([&queue, &stopFlag, &finished]()
            {
                TReferencePtr<Task> task;
                queue.PopBlocking(task, stopFlag);
                finished.store(true);
            });

            // 待機側が述語評価と wait 突入の間にいるタイミングを狙う。
            stopFlag.store(true);
            queue.NotifyAll();

            waiter.join();
            EXPECT_TRUE(finished.load()) << "attempt=" << attempt;
        }
    }
}
