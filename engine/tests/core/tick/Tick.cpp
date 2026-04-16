#include <gtest/gtest.h>
#include "core/tick/TickManager.hpp"
#include "core/tick/TickObject.hpp"

using namespace slug::core;
namespace
{
#if 0
class TestTickObject : public TickObject
{
public:
    TestTickObject(UUID uuid)
        : TickObject(uuid)
    {}

    void Tick(const TickParam& tickParam) override
    {
        tickCalled = true;
    }

    bool tickCalled = false;
};

class OrderTickObject : public TickObject
{
public:
    OrderTickObject(UUID uuid, std::vector<UUID>& order)
        : TickObject(uuid)
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
    AtomicTickObject(UUID uuid, std::atomic<int>& c)
        : TickObject(uuid), counter(c)
    {}

    void Tick(const TickParam&) override
    {
        counter.fetch_add(1);
    }

    std::atomic<int>& counter;
};

class GroupTestTickObject : public TickObject
{
public:
    GroupTestTickObject(UUID uuid = {})
        : TickObject(uuid)
    {}

    void Tick(const TickParam&) override
    {
        tickCount++;
    }

    int tickCount = 0;
};

enum class TestTickGroup
{
    GroupTest1,
    GroupTest2,
    GroupTest3
};

TEST(TickFunctionTest, AddDependency)
{
    UUID parentUUID = {};
    UUID childUUID = {};
    TestTickObject parent(parentUUID);
    TestTickObject child(childUUID);

    parent.AddPrimaryTickDependency(&child);

    EXPECT_TRUE(parent.HasPrimaryTickDependency(childUUID));
}

TEST(TickFunctionTest, DeleteDependency)
{
    UUID parentUUID = {};
    UUID childUUID = {};
    TestTickObject parent(parentUUID);
    TestTickObject child(childUUID);

    parent.AddPrimaryTickDependency(&child);
    EXPECT_TRUE(parent.HasPrimaryTickDependency(childUUID));

    parent.DeletePrimaryTickDependency(childUUID);

    EXPECT_FALSE(parent.HasPrimaryTickDependency(childUUID));
}

TEST(TickFunctionTest, ExecuteCallsTick)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());

    UUID uuid = {};
    TestTickObject object(uuid);

    TickParam param;
    param.crrentTime = 1.0;
    param.deltaTime = 0.1f;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);

    object.GetPrimaryTickFunction()->Execute(param);
    EXPECT_TRUE(object.tickCalled);
}

TEST(TickManagerTest, RegisterAddsFunction)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    TickManager manager(taskSystem);

    UUID uuid = {};
    TestTickObject object(uuid);

    manager.Register(object.GetPrimaryTickFunction());

    TickParam param;
    param.crrentTime = 1.0;
    param.deltaTime = 0.1f;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);

    manager.Execute(param);

    EXPECT_TRUE(object.tickCalled);
}

TEST(TickFunctionTest, ExecuteCallsDependents)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());

    UUID parentUUID = {};
    UUID childUUID = {};
    TestTickObject parent(parentUUID);
    TestTickObject child(childUUID);

    parent.AddPrimaryTickDependency(&child);

    TickParam param;
    param.crrentTime = 1.0;
    param.deltaTime = 0.1f;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);

    parent.GetPrimaryTickFunction()->Execute(param);
    EXPECT_TRUE(parent.tickCalled);
    EXPECT_TRUE(child.tickCalled);
}

TEST(TickFunctionTest, DependencyExecutionOrder)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());

    UUID parentUUID = {};
    UUID childUUID = {};

    std::vector<UUID> order;

    OrderTickObject parent(parentUUID, order);
    OrderTickObject child(childUUID, order);

    parent.AddPrimaryTickDependency(&child);

    TickParam param;
    param.crrentTime = 1.0;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);

    parent.GetPrimaryTickFunction()->Execute(param);

    ASSERT_EQ(order.size(), 2);
    EXPECT_EQ(order[0], parentUUID);
    EXPECT_EQ(order[1], childUUID);
}

TEST(TickFunctionTest, ParallelExecutionSafety)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    UUID parentUUID = {};
    std::atomic<int> counter = 0;
    AtomicTickObject parent(parentUUID, counter);

    std::vector<std::unique_ptr<AtomicTickObject>> children;
    for (int i = 0; i < 100; ++i)
    {
        children.emplace_back(
            std::make_unique<AtomicTickObject>(UUID(), counter)
        );
        parent.AddPrimaryTickDependency(children.back().get());
    }

    TickParam param;
    param.crrentTime = 1.0;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);

    parent.GetPrimaryTickFunction()->Execute(param);
    EXPECT_EQ(counter.load(), 101);
}

TEST(TickFunctionTest, TickIntervalRespected)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    std::vector<UUID> order;

    OrderTickObject obj(UUID(), order);

    obj.GetPrimaryTickFunction()->tickInterval = 1.0f;

    TickParam param;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);

    param.crrentTime = 0.5;
    obj.GetPrimaryTickFunction()->Execute(param);
    EXPECT_TRUE(order.empty());

    param.crrentTime = 1.5;
    obj.GetPrimaryTickFunction()->Execute(param);
    EXPECT_EQ(order.size(), 1);
}

TEST(TickManagerTest, PreventDuplicateRegister)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    TickManager manager(taskSystem);

    UUID uuid;
    TestTickObject tickObject(uuid);

    manager.Register(tickObject.GetPrimaryTickFunction());
    manager.Register(tickObject.GetPrimaryTickFunction());

    TickParam param;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);
    param.crrentTime = 1.0;

    manager.Execute(param);

    EXPECT_TRUE(tickObject.tickCalled); // 1回のみ呼ばれる想定
}

TEST(TickFunctionTest, ReparentingDependency)
{
    UUID parentUUID1 = {};
    UUID parentUUID2 = {};
    UUID childUUID = {};
    TestTickObject parent1(parentUUID1);
    TestTickObject parent2(parentUUID2);
    TestTickObject child(childUUID);

    parent1.AddPrimaryTickDependency(&child);
    EXPECT_TRUE(parent1.HasPrimaryTickDependency(childUUID));

    parent2.AddPrimaryTickDependency(&child);

    EXPECT_FALSE(parent1.HasPrimaryTickDependency(childUUID));
    EXPECT_TRUE(parent2.HasPrimaryTickDependency(childUUID));
}

TEST(TickFunctionTest, CleanRemovesExpired)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    UUID uuid;
    TestTickObject parent(uuid);

    UUID childUUID = {};
    {
        auto child = std::make_unique<TestTickObject>(childUUID);
        parent.AddPrimaryTickDependency(child.get());
    }

    TickParam param;
    param.crrentTime = 1.0;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);

    parent.GetPrimaryTickFunction()->Execute(param);
    EXPECT_FALSE(parent.HasPrimaryTickDependency(childUUID));
}

TEST(TickManagerTest, ExecuteOnlyTargetGroup)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    TickManager manager(taskSystem);

    GroupTestTickObject defaultObj;
    GroupTestTickObject customObj;

    // customグループに変更
    customObj.GetPrimaryTickFunction()->type = SLUG_NAMEOF(TestTickGroup::GroupTest1);

    manager.Register(defaultObj.GetPrimaryTickFunction());
    manager.Register(customObj.GetPrimaryTickFunction());

    TickParam param;
    param.taskSystem = taskSystem;
    param.crrentTime = 1.0;

    // Defaultグループ実行
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);
    manager.Execute(param);

    EXPECT_EQ(defaultObj.tickCount, 1);
    EXPECT_EQ(customObj.tickCount, 0);
}

TEST(TickManagerTest, ExecuteCustomGroup)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    TickManager manager(taskSystem);

    GroupTestTickObject defaultObj;
    GroupTestTickObject customObj;

    customObj.GetPrimaryTickFunction()->type = SLUG_NAMEOF(TestTickGroup::GroupTest1);

    manager.Register(defaultObj.GetPrimaryTickFunction());
    manager.Register(customObj.GetPrimaryTickFunction());

    TickParam param;
    param.taskSystem = taskSystem;
    param.crrentTime = 1.0;

    param.groupType = SLUG_NAMEOF(TestTickGroup::GroupTest1);
    manager.Execute(param);

    EXPECT_EQ(defaultObj.tickCount, 0);
    EXPECT_EQ(customObj.tickCount, 1);
}

TEST(TickManagerTest, MultipleGroupsSequentialExecution)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    TickManager manager(taskSystem);

    GroupTestTickObject objA;
    GroupTestTickObject objB;
    GroupTestTickObject objC;

    objB.GetPrimaryTickFunction()->type = SLUG_NAMEOF(TestTickGroup::GroupTest1);
    objC.GetPrimaryTickFunction()->type = SLUG_NAMEOF(TestTickGroup::GroupTest2);

    manager.Register(objA.GetPrimaryTickFunction());
    manager.Register(objB.GetPrimaryTickFunction());
    manager.Register(objC.GetPrimaryTickFunction());

    TickParam param;
    param.taskSystem = taskSystem;
    param.crrentTime = 1.0;

    // Default
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);
    manager.Execute(param);

    // Physics
    param.groupType = SLUG_NAMEOF(TestTickGroup::GroupTest1);
    manager.Execute(param);

    // Render
    param.groupType = SLUG_NAMEOF(TestTickGroup::GroupTest2);
    manager.Execute(param);

    EXPECT_EQ(objA.tickCount, 1);
    EXPECT_EQ(objB.tickCount, 1);
    EXPECT_EQ(objC.tickCount, 1);
}

TEST(TickManagerTest, ExecuteNonExistingGroup)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    TickManager manager(taskSystem);

    GroupTestTickObject obj;
    manager.Register(obj.GetPrimaryTickFunction());

    TickParam param;
    param.taskSystem = taskSystem;
    param.crrentTime = 1.0;

    param.groupType = SLUG_NAMEOF(TestTickGroup::GroupTest2);
    manager.Execute(param);

    EXPECT_EQ(obj.tickCount, 0);
}

TEST(TickManagerTest, GroupRemovedWhenEmpty)
{
    TaskSystemPtr taskSystem = new TaskSystem(TaskSystemDesc());
    TickManager manager(taskSystem);
    {
        GroupTestTickObject obj;
        manager.Register(obj.GetPrimaryTickFunction());
    }

    TickParam param;
    param.taskSystem = taskSystem;
    param.groupType = SLUG_NAMEOF(SystemTickGroupType::Default);
    param.crrentTime = 1.0;

    // Cleanで削除される
    manager.Execute(param);

    // 2回目呼んでもクラッシュしないこと確認
    manager.Execute(param);

    SUCCEED();
}
#endif

}
