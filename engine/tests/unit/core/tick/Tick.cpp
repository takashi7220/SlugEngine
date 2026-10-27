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
    OrderTickObject(std::vector<RuntimeID>& order)
        : TickObject()
        , m_order(order)
    {}

    void Tick(const TickParam&) override
    {
        m_order.push_back(m_primaryTickFunction->id);
    }

private:
    std::vector<RuntimeID>& m_order;
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

        EXPECT_TRUE(parent.HasPrimaryTickDependency(child.GetPrimaryTickFunction()->id));
    }
}

TEST(TickFunctionTest, DeleteDependency)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TestTickObject parent;
        TestTickObject child;
        RuntimeID childRuntimeID = child.GetPrimaryTickFunction()->id;

        parent.AddPrimaryTickDependency(&child);
        EXPECT_TRUE(parent.HasPrimaryTickDependency(childRuntimeID));

        parent.DeletePrimaryTickDependency(childRuntimeID);
        EXPECT_FALSE(parent.HasPrimaryTickDependency(childRuntimeID));
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
        std::vector<RuntimeID> order;
        OrderTickObject parent(order);
        OrderTickObject child(order);
        RuntimeID parentRuntimeID = parent.GetPrimaryTickFunction()->id;
        RuntimeID childRuntimeID = child.GetPrimaryTickFunction()->id;

        parent.AddPrimaryTickDependency(&child);
        auto param = MakeParam(sys);
        parent.GetPrimaryTickFunction()->Execute(param);

        ASSERT_EQ(order.size(), 2u);
        EXPECT_EQ(order[0], parentRuntimeID);
        EXPECT_EQ(order[1], childRuntimeID);
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
        RuntimeID childRuntimeID = child.GetPrimaryTickFunction()->id;

        parent1.AddPrimaryTickDependency(&child);
        EXPECT_TRUE(parent1.HasPrimaryTickDependency(childRuntimeID));

        parent2.AddPrimaryTickDependency(&child);
        EXPECT_FALSE(parent1.HasPrimaryTickDependency(childRuntimeID));
        EXPECT_TRUE(parent2.HasPrimaryTickDependency(childRuntimeID));
    }
}

TEST(TickFunctionTest, CleanRemovesExpiredDependents)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TestTickObject parent;
        RuntimeID childRuntimeID;

        {
            TestTickObject child;
            childRuntimeID = child.GetPrimaryTickFunction()->id;
            parent.AddPrimaryTickDependency(&child);
        }

        auto param = MakeParam(sys);
        parent.GetPrimaryTickFunction()->Execute(param);

        EXPECT_FALSE(parent.HasPrimaryTickDependency(childRuntimeID));
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

//------------------------------------------------------------------------------
// 既存グループへの追加登録
//------------------------------------------------------------------------------
// Register の「既存グループあり」経路はローカルコピーに push すると
// 2つ目以降の関数が捨てられ、Tick が呼ばれなくなる。
TEST(TickManagerTest, RegisterSecondFunctionIntoExistingGroup)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        TestTickObject first;
        TestTickObject second;

        manager.Register(first.GetPrimaryTickFunction());
        manager.Register(second.GetPrimaryTickFunction());

        auto param = MakeParam(sys);
        manager.Execute(param);

        EXPECT_EQ(first.tickCount, 1);
        EXPECT_EQ(second.tickCount, 1);
    }
}

TEST(TickManagerTest, RegisterManyFunctionsIntoSameGroup)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        std::vector<std::unique_ptr<TestTickObject>> objects;
        for (int i = 0; i < 32; ++i)
        {
            objects.emplace_back(std::make_unique<TestTickObject>());
            manager.Register(objects.back()->GetPrimaryTickFunction());
        }

        auto param = MakeParam(sys);
        manager.Execute(param);

        for (size_t i = 0; i < objects.size(); ++i)
        {
            EXPECT_EQ(objects[i]->tickCount, 1) << "index=" << i;
        }
    }
}

// 同一グループに複数登録しても、重複登録は1回に抑えられること。
TEST(TickManagerTest, DuplicateRegisterIntoExistingGroupIsIgnored)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        TestTickObject first;
        TestTickObject second;

        manager.Register(first.GetPrimaryTickFunction());
        manager.Register(second.GetPrimaryTickFunction());
        manager.Register(second.GetPrimaryTickFunction());

        auto param = MakeParam(sys);
        manager.Execute(param);

        EXPECT_EQ(first.tickCount, 1);
        EXPECT_EQ(second.tickCount, 1);
    }
}

TEST(TickManagerTest, RegisterNullptrIsIgnored)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        manager.Register(nullptr);

        EXPECT_FALSE(manager.HasGroup(SLUG_NAMEOF(SystemTickGroupType::Default)));
    }
}

// TickParam を値で捕まえるとタスクが TaskSystem への強参照を持ち、
// TaskSystem -> WorkQueue -> Task -> ラムダ -> TaskSystem の所有サイクルになる。
// 呼び出し側が参照を手放しても TaskSystem がワーカー上で生き残るため、
// スコープを抜けた時点で解放が終わっていない。
TEST(TickManagerTest, ExecuteDoesNotLeaveTaskSystemReferencesBehind)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        for (int i = 0; i < 200; ++i)
        {
            auto sys = MakeTaskSystem();
            TickManager manager(sys.get());
            TestTickObject obj;
            manager.Register(obj.GetPrimaryTickFunction());

            auto param = MakeParam(sys);
            manager.Execute(param);

            ASSERT_EQ(obj.tickCount, 1) << "iteration=" << i;

            // Execute から戻った時点で TaskSystem を掴んでいるのは
            // sys / param.taskSystem / manager.m_taskSystem の3つだけであること。
            // タスク側に強参照が残っていればこれより多くなる。
            ASSERT_EQ(sys->GetReferenceCount(), 3u) << "iteration=" << i;
        }
    }
}

TEST(TickManagerTest, RepeatedExecuteIsStable)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        auto sys = MakeTaskSystem();
        TickManager manager(sys.get());

        TestTickObject first;
        TestTickObject second;
        manager.Register(first.GetPrimaryTickFunction());
        manager.Register(second.GetPrimaryTickFunction());

        auto param = MakeParam(sys);
        for (int i = 0; i < 100; ++i)
        {
            // CheckAndUpdateInterval が毎回経過したと判定するよう時刻を進める。
            param.crrentTime = static_cast<double>(i + 1);
            manager.Execute(param);
        }

        EXPECT_EQ(first.tickCount, 100);
        EXPECT_EQ(second.tickCount, 100);
    }
}
