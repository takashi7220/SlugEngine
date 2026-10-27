#include <gtest/gtest.h>

#include "core/container/Array.hpp"
#include "core/container/UnorderedSet.hpp"
#include "core/container/Vector.hpp"
#include "core/thread/Thread.hpp"
#include "core/uuid/RuntimeID.hpp"
#include "core/window/WindowEventListner.hpp"

using namespace slug::core;

namespace
{

TEST(RuntimeIDTest, DefaultIDIsInvalidAndEightBytes)
{
    const RuntimeID id;

    EXPECT_EQ(sizeof(RuntimeID), 8u);
    EXPECT_EQ(id.GetValue(), 0u);
    EXPECT_FALSE(id.IsValid());
}

TEST(RuntimeIDTest, GeneratedIDsAreValidAndIncreasing)
{
    RuntimeID previous = RuntimeID::Generate();
    ASSERT_TRUE(previous.IsValid());
    for (int i = 0; i < 1024; ++i)
    {
        RuntimeID next;
        ASSERT_TRUE(RuntimeID::TryGenerate(next));
        EXPECT_GT(next.GetValue(), previous.GetValue());
        previous = next;
    }
}

TEST(RuntimeIDTest, CopiesWorkAsUnorderedSetKeys)
{
    const RuntimeID id = RuntimeID::Generate();
    const RuntimeID copy = id;
    TUnorderedSet<RuntimeID, RuntimeIDHash> values;
    values.insert(id);
    values.insert(copy);
    values.insert(RuntimeID::Generate());

    EXPECT_EQ(id, copy);
    EXPECT_EQ(id.GetHash(), copy.GetHash());
    EXPECT_EQ(RuntimeIDHash{}(id), id.GetHash());
    EXPECT_EQ(values.size(), 2u);
}

TEST(RuntimeIDTest, ConcurrentGenerationDoesNotDuplicateIDs)
{
    constexpr size_t ThreadCount = 8;
    constexpr size_t IDsPerThread = 4096;
    TArray<TVector<RuntimeID>, ThreadCount> results;
    TArray<Thread, ThreadCount> threads;
    for (size_t index = 0; index < ThreadCount; ++index)
    {
        results[index].reserve(IDsPerThread);
        threads[index] = Thread([&, index]()
        {
            for (size_t i = 0; i < IDsPerThread; ++i)
            {
                results[index].push_back(RuntimeID::Generate());
            }
        });
    }
    for (Thread& thread : threads)
    {
        thread.join();
    }

    TUnorderedSet<RuntimeID, RuntimeIDHash> unique;
    for (const auto& batch : results)
    {
        for (RuntimeID id : batch)
        {
            ASSERT_TRUE(id.IsValid());
            ASSERT_TRUE(unique.insert(id).second);
        }
    }
    EXPECT_EQ(unique.size(), ThreadCount * IDsPerThread);
}

TEST(RuntimeIDTest, WindowCallbacksReceiveDistinctIDs)
{
    WindowEventListner listener;
    WindowEventListner::RegisterParam param;
    param.targetWindow = WindowID::Generate();
    param.type = WindowEventType::WindowClose;
    int callCount = 0;
    param.callback = [&](const WindowEventBase*) { ++callCount; };
    const WindowCallbackID first = listener.RegisterCallback(param);
    const WindowCallbackID second = listener.RegisterCallback(param);

    EXPECT_TRUE(first.IsValid());
    EXPECT_TRUE(second.IsValid());
    EXPECT_NE(first, second);
    const WindowCloseEvent event(param.targetWindow);
    listener.SendEvent(&event);
    EXPECT_EQ(callCount, 2);

    listener.UnregisterCallback(first);
    listener.SendEvent(&event);
    EXPECT_EQ(callCount, 3);
}

}
