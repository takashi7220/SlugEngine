#include <gtest/gtest.h>
#include "core/memory/WeakReferencePtr.hpp"

using namespace slug::core;
namespace
{
class TestObject : public ReferenceObject
{
public:
    __declspec(noinline) TestObject()
    {
        ++constructed_count;
    }
    __declspec(noinline) ~TestObject()
    {
        ++destructed_count;
    }

    static void ResetCounters()
    {
        constructed_count.store(0);
        destructed_count.store(0);
    }
    static int Constructed()
    {
        return constructed_count.load();
    }
    static int Destructed()
    {
        return destructed_count.load();
    }

private:
    static std::atomic<int> constructed_count;
    static std::atomic<int> destructed_count;
};

std::atomic<int> TestObject::constructed_count { 0 };
std::atomic<int> TestObject::destructed_count { 0 };

//------------------------------------------------------------------------------
// 基本：デフォルト/ヌル構築は expired & lock 失敗
//------------------------------------------------------------------------------
TEST(TWeakReferencePtrTest, DefaultAndNullConstruction)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TWeakReferencePtr<TestObject> w0;
        EXPECT_TRUE(w0.expired());
        EXPECT_EQ(w0.get_unsafe(), nullptr);
        auto s0 = w0.lock();
        EXPECT_EQ(s0.get(), nullptr);

        TWeakReferencePtr<TestObject> w1(nullptr);
        EXPECT_TRUE(w1.expired());
        EXPECT_EQ(w1.get_unsafe(), nullptr);
        auto s1 = w1.lock();
        EXPECT_EQ(s1.get(), nullptr);
    }
}

//------------------------------------------------------------------------------
// 強参照から弱参照を作る → lock で強参照取得できる
//------------------------------------------------------------------------------
TEST(TWeakReferencePtrTest, ConstructFromStrongAndLock)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TestObject::ResetCounters();
        {
            // 生オブジェクトを生成（ReferenceObject のコンストラクタで ControlBlock 生成）
            auto* raw = new TestObject();
            EXPECT_EQ(TestObject::Constructed(), 1);

            // 強参照を作る（AddRef される）
            TReferencePtr<TestObject> strong(raw);
            EXPECT_EQ(strong.get(), raw);

            // 強参照から弱参照を作る
            TWeakReferencePtr<TestObject> weak_from_strong(strong);
            EXPECT_FALSE(weak_from_strong.expired());

            // lock で昇格
            auto locked = weak_from_strong.lock();
            EXPECT_TRUE(locked.get() != nullptr);
            EXPECT_EQ(locked.get(), raw);

            // locked がスコープを出ると strong--（0にはならない：strong 本体が生きている）
        }
        // strong がスコープアウトした時点で raw は delete されるはず
        EXPECT_EQ(TestObject::Destructed(), 1);
    }
}

//------------------------------------------------------------------------------
// 強参照破棄後は expired / lock 失敗
//------------------------------------------------------------------------------
TEST(TWeakReferencePtrTest, ExpiredAfterStrongReleased)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TestObject::ResetCounters();
        TWeakReferencePtr<TestObject> weak;

        {
            auto* raw = new TestObject();
            TReferencePtr<TestObject> strong(raw); // strong:1
            weak = strong;                          // weak:1
            EXPECT_FALSE(weak.expired());

            // 昇格できる
            auto locked1 = weak.lock();
            EXPECT_TRUE(locked1.get() == raw);
        }
        // strong:0 → ReferenceObject::Release() が delete this を実行
        EXPECT_EQ(TestObject::Destructed(), 1);

        // 以降は expired / lock 失敗
        EXPECT_TRUE(weak.expired());
        auto locked2 = weak.lock();
        EXPECT_EQ(locked2.get(), nullptr);
    }
}

//------------------------------------------------------------------------------
// コピー構築／コピー代入：同じ対象を指す・有効性維持
//------------------------------------------------------------------------------
TEST(TWeakReferencePtrTest, CopyConstructionAndAssignment)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        auto* raw = new TestObject();
        TReferencePtr<TestObject> strong(raw); // strong:1

        TWeakReferencePtr<TestObject> w1(strong);
        EXPECT_FALSE(w1.expired());

        TWeakReferencePtr<TestObject> w2(w1); // コピー構築
        EXPECT_FALSE(w2.expired());
        EXPECT_EQ(w1.get_unsafe(), w2.get_unsafe());

        TWeakReferencePtr<TestObject> w3;
        w3 = w2; // コピー代入
        EXPECT_FALSE(w3.expired());
        EXPECT_EQ(w2.get_unsafe(), w3.get_unsafe());

        // 強参照破棄後は全て expired
        strong = nullptr;
        EXPECT_TRUE(w1.expired());
        EXPECT_TRUE(w2.expired());
        EXPECT_TRUE(w3.expired());
        // raw は delete 済み
    }
}

//------------------------------------------------------------------------------
// ムーブ構築／ムーブ代入：移動元はヌル化、移動先は有効
//------------------------------------------------------------------------------
TEST(TWeakReferencePtrTest, MoveConstructionAndAssignment)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        auto* raw = new TestObject();
        TReferencePtr<TestObject> strong(raw);

        TWeakReferencePtr<TestObject> w1(strong);
        EXPECT_FALSE(w1.expired());

        TWeakReferencePtr<TestObject> w2(std::move(w1));
        EXPECT_EQ(w1.get_unsafe(), nullptr);
        EXPECT_FALSE(w2.expired());
        EXPECT_EQ(w2.get_unsafe(), raw);

        TWeakReferencePtr<TestObject> w3;
        w3 = std::move(w2);
        EXPECT_EQ(w2.get_unsafe(), nullptr);
        EXPECT_FALSE(w3.expired());
        EXPECT_EQ(w3.get_unsafe(), raw);

        // 強参照破棄 → weak は expired
        strong = nullptr;
        EXPECT_TRUE(w3.expired());
    }
}

//------------------------------------------------------------------------------
// swap の検証
//------------------------------------------------------------------------------
TEST(TWeakReferencePtrTest, Swap)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        auto* a = new TestObject();
        auto* b = new TestObject();

        TReferencePtr<TestObject> sa(a);
        TReferencePtr<TestObject> sb(b);

        TWeakReferencePtr<TestObject> wa(sa);
        TWeakReferencePtr<TestObject> wb(sb);

        EXPECT_EQ(wa.get_unsafe(), a);
        EXPECT_EQ(wb.get_unsafe(), b);

        wa.swap(wb);
        EXPECT_EQ(wa.get_unsafe(), b);
        EXPECT_EQ(wb.get_unsafe(), a);

        // 強参照破棄後は両方 expired
        sa = nullptr;
        sb = nullptr;
        EXPECT_TRUE(wa.expired());
        EXPECT_TRUE(wb.expired());
    }
}

//------------------------------------------------------------------------------
// nullptr 代入で弱参照を解放：その後の lock は失敗
//------------------------------------------------------------------------------
TEST(TWeakReferencePtrTest, AssignNullptrReleasesWeak)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {

        TestObject::ResetCounters();
        auto* raw = new TestObject();
        TReferencePtr<TestObject> strong(raw);

        TWeakReferencePtr<TestObject> w(strong);
        EXPECT_FALSE(w.expired());
        EXPECT_NE(w.get_unsafe(), nullptr);

        // 弱参照を nullptr 代入で解放
        w = nullptr;
        EXPECT_EQ(w.get_unsafe(), nullptr);
        EXPECT_TRUE(w.expired());
        auto locked = w.lock();
        EXPECT_EQ(locked.get(), nullptr);

        // 強参照を残しておけばオブジェクトはまだ削除されない
        EXPECT_EQ(TestObject::Destructed(), 0);

        // 強参照破棄で delete
        strong = nullptr;
        EXPECT_EQ(TestObject::Destructed(), 1);
    }
}

//------------------------------------------------------------------------------
// 生ポインタから直接弱参照を作る（強参照がなければ expired）
//------------------------------------------------------------------------------
TEST(TWeakReferencePtrTest, ConstructWeakFromRawPointer)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        auto* raw = new TestObject();
        // 強参照なしに弱参照だけを作ると strong==0 のため expired
        {
            TWeakReferencePtr<TestObject> w_from_raw(raw);
            EXPECT_TRUE(w_from_raw.expired());
            EXPECT_EQ(w_from_raw.lock().get(), nullptr);
        }
        raw->AddRef();

        // 強参照を後から作れば、別の弱参照は有効化される
        {
            TReferencePtr<TestObject> strong(raw);
            TWeakReferencePtr<TestObject> w2(strong);
            EXPECT_FALSE(w2.expired());
            EXPECT_NE(w2.lock().get(), nullptr);
        }

        // 最後は強参照を解放して delete
        {
            TReferencePtr<TestObject> tmp(raw); // AddRef（2）
            tmp = nullptr;                      // Release（1）
        }
        raw->Release();
    }
}
}
