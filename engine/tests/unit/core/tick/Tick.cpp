#include <gtest/gtest.h>
#include "core/tick/TickManager.hpp"
#include "core/tick/TickObject.hpp"

using namespace slug::core;

namespace
{

class TestTickObject : public TickObject
{
public:
    TestTickObject() : TickObject() {}

    void Tick(const TickParam&) override
    {
        tickCount++;
    }

    int tickCount = 0;
};

class OrderTickObject : public TickObject
{
public:
    OrderTickObject(std::vector<UUID>& order)
        : TickObject()
        , m_order(order)
    {}

    void Tick(const TickParam&) override
    {
        m_order.push_back(m_primaryTickFunction->uuid);
    }

private:
    std::vector<UUID>& m_order;
};

class AtomicTickObject : public TickObject
{
public:
    AtomicTickObject(std::atomic<int>& counter)
        : TickObject()
        , m_counter(counter)
    {}

    void Tick(const TickParam&) override
    {
        m_counter.fetch_add(1, std::memory_order_relaxed);
    }

private:
    std::atomic<int>& m_counter;
};

TaskSystemPtr MakeTaskSystem()
{
    return TaskSystemPtr(new TaskSystem(TaskSystem::Desc{ .workerCount = std::max(2u, std::thread::hardware_concurrency()) }));
}

TickParam MakeParam(TaskSystemPtr& sys, double currentTime = 1.0)
{
    TickParam param;
    param.crrentTime = currentTime;
    param.deltaTime = 0.016f;
    param.taskSystem = sys;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);
    return param;
}

} // namespace

// ---------------------------------------------------------------------------
// TickFunction
// ---------------------------------------------------------------------------

TEST(TickFunctionTest, AddDependency)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TestTickObject parent;
        TestTickObject child;

        parent.AddPrimaryTickDependency(&child);

        EXPECT_TRUE(parent.HasPrimaryTickDependency(child.GetPrimaryTickFunction()->uuid));
    }
}

TEST(TickFunctionTest, DeleteDependency)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TestTickObject parent;
        TestTickObject child;
        UUID childUUID = child.GetPrimaryTickFunction()->uuid;

        parent.AddPrimaryTickDependency(&child);
        EXPECT_TRUE(parent.HasPrimaryTickDependency(childUUID));

        parent.DeletePrimaryTickDependency(childUUID);
        EXPECT_FALSE(parent.HasPrimaryTickDependency(childUUID));
    }
}

TEST(TickFunctionTest, ExecuteCallsTick)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TestTickObject obj;
        auto param = MakeParam(sys);

        obj.GetPrimaryTickFunction()->Execute(param);

        EXPECT_EQ(obj.tickCount, 1);
    }
}

TEST(TickFunctionTest, ExecuteCallsDependents)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TestTickObject parent;
        TestTickObject child;
        parent.AddPrimaryTickDependency(&child);
        auto param = MakeParam(sys);

        parent.GetPrimaryTickFunction()->Execute(param);

        EXPECT_EQ(parent.tickCount, 1);
        EXPECT_EQ(child.tickCount, 1);
    }
}

TEST(TickFunctionTest, DependencyExecutionOrder)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        std::vector<UUID> order;
        OrderTickObject parent(order);
        OrderTickObject child(order);
        UUID parentUUID = parent.GetPrimaryTickFunction()->uuid;
        UUID childUUID = child.GetPrimaryTickFunction()->uuid;

        parent.AddPrimaryTickDependency(&child);
        auto param = MakeParam(sys);
        parent.GetPrimaryTickFunction()->Execute(param);

        ASSERT_EQ(order.size(), 2u);
        EXPECT_EQ(order[0], parentUUID);
        EXPECT_EQ(order[1], childUUID);
    }
}

TEST(TickFunctionTest, ParallelExecutionSafety)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        std::atomic<int> counter { 0 };
        AtomicTickObject parent(counter);

        std::vector<std::unique_ptr<AtomicTickObject>> children;
        for (int i = 0; i < 100; ++i)
        {
            children.emplace_back(std::make_unique<AtomicTickObject>(counter));
            parent.AddPrimaryTickDependency(children.back().get());
        }

        auto param = MakeParam(sys);
        parent.GetPrimaryTickFunction()->Execute(param);

        EXPECT_EQ(counter.load(), 101);
    }
}

TEST(TickFunctionTest, TickIntervalSkipsWhenNotElapsed)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TestTickObject obj;
        obj.GetPrimaryTickFunction()->tickInterval = 1.0f;

        auto param = MakeParam(sys, 0.5);
        obj.GetPrimaryTickFunction()->Execute(param);

        EXPECT_EQ(obj.tickCount, 0);
    }
}

TEST(TickFunctionTest, TickIntervalExecutesWhenElapsed)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TestTickObject obj;
        obj.GetPrimaryTickFunction()->tickInterval = 1.0f;

        auto param = MakeParam(sys, 0.5);
        obj.GetPrimaryTickFunction()->Execute(param);
        EXPECT_EQ(obj.tickCount, 0);

        param.crrentTime = 1.5;
        obj.GetPrimaryTickFunction()->Execute(param);
        EXPECT_EQ(obj.tickCount, 1);
    }
}

TEST(TickFunctionTest, ReparentingDependency)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TestTickObject parent1;
        TestTickObject parent2;
        TestTickObject child;
        UUID childUUID = child.GetPrimaryTickFunction()->uuid;

        parent1.AddPrimaryTickDependency(&child);
        EXPECT_TRUE(parent1.HasPrimaryTickDependency(childUUID));

        parent2.AddPrimaryTickDependency(&child);
        EXPECT_FALSE(parent1.HasPrimaryTickDependency(childUUID));
        EXPECT_TRUE(parent2.HasPrimaryTickDependency(childUUID));
    }
}

TEST(TickFunctionTest, CleanRemovesExpiredDependents)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TestTickObject parent;
        UUID childUUID;

        {
            TestTickObject child;
            childUUID = child.GetPrimaryTickFunction()->uuid;
            parent.AddPrimaryTickDependency(&child);
        }

        auto param = MakeParam(sys);
        parent.GetPrimaryTickFunction()->Execute(param);

        EXPECT_FALSE(parent.HasPrimaryTickDependency(childUUID));
    }
}

// ---------------------------------------------------------------------------
// TickManager
// ---------------------------------------------------------------------------

TEST(TickManagerTest, RegisterAddsToGroup)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());
        TestTickObject obj;

        manager.Register(obj.GetPrimaryTickFunction());

        EXPECT_TRUE(manager.HasGroup(SLUG_NAMEOF(SystemTickGroupType::Default)));
    }
}

TEST(TickManagerTest, ExecuteCallsTick)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());
        TestTickObject obj;
        manager.Register(obj.GetPrimaryTickFunction());

        auto param = MakeParam(sys);
        manager.Execute(param);

        EXPECT_EQ(obj.tickCount, 1);
    }
}

TEST(TickManagerTest, PreventDuplicateRegister)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());
        TestTickObject obj;

        manager.Register(obj.GetPrimaryTickFunction());
        manager.Register(obj.GetPrimaryTickFunction());

        auto param = MakeParam(sys);
        manager.Execute(param);

        EXPECT_EQ(obj.tickCount, 1);
    }
}

TEST(TickManagerTest, ExecuteOnlyTargetGroup)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        TestTickObject defaultObj;
        TestTickObject customObj;
        customObj.GetPrimaryTickFunction()->type = "CustomGroup";

        manager.Register(defaultObj.GetPrimaryTickFunction());
        manager.Register(customObj.GetPrimaryTickFunction());

        auto param = MakeParam(sys);
        param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);
        manager.Execute(param);

        EXPECT_EQ(defaultObj.tickCount, 1);
        EXPECT_EQ(customObj.tickCount, 0);
    }
}

TEST(TickManagerTest, ExecuteCustomGroup)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        TestTickObject defaultObj;
        TestTickObject customObj;
        customObj.GetPrimaryTickFunction()->type = "CustomGroup";

        manager.Register(defaultObj.GetPrimaryTickFunction());
        manager.Register(customObj.GetPrimaryTickFunction());

        auto param = MakeParam(sys);
        param.groupType = "CustomGroup";
        manager.Execute(param);

        EXPECT_EQ(defaultObj.tickCount, 0);
        EXPECT_EQ(customObj.tickCount, 1);
    }
}

TEST(TickManagerTest, MultipleGroupsSequentialExecution)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        TestTickObject objA;
        TestTickObject objB;
        TestTickObject objC;
        objB.GetPrimaryTickFunction()->type = "GroupB";
        objC.GetPrimaryTickFunction()->type = "GroupC";

        manager.Register(objA.GetPrimaryTickFunction());
        manager.Register(objB.GetPrimaryTickFunction());
        manager.Register(objC.GetPrimaryTickFunction());

        auto param = MakeParam(sys);

        param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);
        manager.Execute(param);

        param.groupType = "GroupB";
        manager.Execute(param);

        param.groupType = "GroupC";
        manager.Execute(param);

        EXPECT_EQ(objA.tickCount, 1);
        EXPECT_EQ(objB.tickCount, 1);
        EXPECT_EQ(objC.tickCount, 1);
    }
}

TEST(TickManagerTest, ExecuteNonExistingGroupDoesNotCrash)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());
        TestTickObject obj;
        manager.Register(obj.GetPrimaryTickFunction());

        auto param = MakeParam(sys);
        param.groupType = "NoSuchGroup";

        EXPECT_NO_THROW(manager.Execute(param));
        EXPECT_EQ(obj.tickCount, 0);
    }
}

TEST(TickManagerTest, CleanGroupPreservesNonEmptyGroup)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        TestTickObject obj;
        manager.Register(obj.GetPrimaryTickFunction());
        EXPECT_TRUE(manager.HasGroup(SLUG_NAMEOF(SystemTickGroupType::Default)));

        manager.CleanGroup();

        EXPECT_TRUE(manager.HasGroup(SLUG_NAMEOF(SystemTickGroupType::Default)));
    }
}
