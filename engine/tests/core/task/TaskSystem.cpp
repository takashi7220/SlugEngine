#include <gtest/gtest.h>
#include "core/task/TaskSystem.hpp"
#include "core/container/Vector.hpp"
#include "core/container/String.hpp"

using namespace slug::core;
TEST(TaskSystem, BasicLaunch)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TaskSystem sys({ .workerCount = std::max(2u, std::thread::hardware_concurrency()) });
        std::atomic<int> flag { 0 };
        auto h = sys.Launch([&]
        {
                flag.store(42, std::memory_order_release);
        });
        sys.Wait(h);
        EXPECT_EQ(flag.load(std::memory_order_acquire), 42);
    }
}


TEST(TaskSystem, DependenciesChain)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TaskSystem sys({ .workerCount = 4 });
        TVector<String> order = {};
        Mutex m;
        auto A = sys.Launch([&order, &m]
        {
            std::scoped_lock lk(m);
            order.push_back("A");
        });
        auto B = sys.Launch([&]
        {
            std::scoped_lock lk(m);
            order.push_back("B");
        }, std::span<const TaskHandle>(&A, 1));
        auto C = sys.Launch([&]
        {
           std::scoped_lock lk(m);
           order.push_back("C");
        }, std::span<const TaskHandle>(&B, 1));
        sys.Wait(C);
        ASSERT_EQ(order.size(), 3u);
        EXPECT_EQ(order[0], "A");
        EXPECT_EQ(order[1], "B");
        EXPECT_EQ(order[2], "C");
    }
}

TEST(TaskSystem, FanInDependencies)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TaskSystem sys({ .workerCount = 4 });
        std::atomic<int> done { 0 };
        auto A = sys.Launch([&]
        {
            done.fetch_add(1, std::memory_order_acq_rel);
         });
        auto B = sys.Launch([&]
        {
            done.fetch_add(1, std::memory_order_acq_rel);
         });
        TaskHandle prereq[2] = { A,B };
        auto C = sys.Launch([&]
        {
            EXPECT_EQ(done.load(std::memory_order_acquire), 2); done.fetch_add(1);
        }, prereq);
        sys.Wait(C);
        EXPECT_EQ(done.load(), 3);
    }
}


TEST(TaskSystem, ParallelForCorrectness)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TaskSystem sys({ .workerCount = 8 });
        const size_t N = 100'000;
        std::vector<int> v(N, 0);
        sys.ParallelFor(0, N, ParallelForOptions(4096), [&](size_t i)
        {
            v[i] = static_cast<int>(i * i % 9973);
        });
        for (size_t i = 0; i < N; ++i)
        {
            EXPECT_EQ(v[i], i * i % 9973);
        }
    }
}


TEST(TaskSystem, FinishedPrereqRegistration)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TaskSystem sys({ .workerCount = 4 });
        auto A = sys.Launch([]
        {});
        sys.Wait(A);
        auto B = sys.Launch([]
        {}, std::span<const TaskHandle>(&A, 1));
        // Should complete immediately (no deadlock)
        EXPECT_NO_THROW(sys.Wait(B));
    }
}


TEST(TaskSystem, ExceptionDoesNotCrashWait)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TaskSystem sys({ .workerCount = 4 });
        auto H = sys.Launch([]
        {
            throw std::runtime_error("boom");
        });
        // Wait should not throw; internal promise captures exception
        EXPECT_NO_THROW(sys.Wait(H));
    }
}

#if 0
TEST(TaskSystem, LargeDAGChainAndJoin)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(LargeDAGChainAndJoin)
    {
        TaskSystem sys({ .workerCount = 8 });
        const int N = 200;
        TVector<TaskHandle> nodes; nodes.reserve(N);
        nodes.push_back(sys.Launch([]{}));
        for (int i = 1; i < N; ++i)
        {
            auto h = sys.Launch([]{}, std::span<const TaskHandle>(&nodes.back(), 1));
            nodes.push_back(h);
        }
        EXPECT_NO_THROW(sys.Wait(nodes.back()));

        auto root = sys.Launch([]{});

        TVector<TaskHandle> leaves; leaves.reserve(32);
        for (int i = 0; i < 32; ++i)
        {
            leaves.push_back(sys.Launch([]{}, std::span<const TaskHandle>(&root, 1)));
        }

        auto join = sys.Launch([]{}, std::span<const TaskHandle>(leaves.data(), leaves.size()));
        EXPECT_NO_THROW(sys.Wait(join));
    }
}
#endif

TEST(TaskSystem, ParallelForStress)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TaskSystem sys({ .workerCount = std::max(2u, std::thread::hardware_concurrency()) });
        const size_t N = 500'000;
        std::vector<uint64_t> v(N);
        sys.ParallelFor(0, N, ParallelForOptions(8192), [&](size_t i)
        {
                v[i] = (i * 1469598103934665603ull) ^ (i >> 3);
        });

        for (size_t i = 0; i < N; ++i)
        {
            ASSERT_EQ(v[i], (i * 1469598103934665603ull) ^ (i >> 3));
        }
    }
}
