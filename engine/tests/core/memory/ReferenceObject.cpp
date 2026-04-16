#include <gtest/gtest.h>
#include "core/memory/ReferencePtr.hpp"

using namespace slug::core;

namespace
{

struct TestObject final : public ReferenceObject
{
    static inline int destroyed_count = 0;
    int value = 0;

    TestObject() = default;
    explicit TestObject(int v) : value(v)
    {
    }
    ~TestObject() override
    {
        ++destroyed_count;
    }
};

struct ReferencePtrFixture : ::testing::Test
{
    void SetUp() override
    {
        TestObject::destroyed_count = 0;
    }
};

TEST_F(ReferencePtrFixture, DefaultConstructIsNull)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TReferencePtr<TestObject> p;
        EXPECT_EQ(p.get(), nullptr);
    }
}

TEST_F(ReferencePtrFixture, NullptrConstructAndAssign)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TReferencePtr<TestObject> p(nullptr);
        EXPECT_EQ(p.get(), nullptr);
        p = nullptr;
        EXPECT_EQ(p.get(), nullptr);
    }
}

TEST_F(ReferencePtrFixture, ConstructFromRawAndDestroyOnScopeExit)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        auto raw = new TestObject(42);

        {
            // 仕様上 T* からのコンストラクタは InternalAddRef() を呼ぶ実装
            TReferencePtr<TestObject> p(raw);
            ASSERT_NE(p.get(), nullptr);
            EXPECT_EQ(p->value, 42);

            // コピーで参照共有
            TReferencePtr<TestObject> p2 = p;
            EXPECT_EQ(p2.get(), raw);
        }
        EXPECT_EQ(TestObject::destroyed_count, 1);
    }
}

TEST_F(ReferencePtrFixture, MoveSemantics)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)

    {
        TReferencePtr<TestObject> p = MakeReference<TestObject>();
        ASSERT_NE(p.get(), nullptr);

        auto* raw = p.get();

        // move ctor
        TReferencePtr<TestObject> q(std::move(p));
        EXPECT_EQ(p.get(), nullptr);
        EXPECT_EQ(q.get(), raw);

        // move assign
        TReferencePtr<TestObject> r;
        r = std::move(q);
        EXPECT_EQ(q.get(), nullptr);
        EXPECT_EQ(r.get(), raw);
    }
}

TEST_F(ReferencePtrFixture, CopySemanticsShareReference)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TReferencePtr<TestObject> p = MakeReference<TestObject>();
        ASSERT_NE(p.get(), nullptr);

        {
            TReferencePtr<TestObject> q;
            q = p; // copy assign
            EXPECT_EQ(q.get(), p.get());
        }
        // q 破棄後も p が保持しているのでまだ delete されない
        EXPECT_EQ(TestObject::destroyed_count, 0);

        // p を破棄して初めて delete される
        p = nullptr;
        EXPECT_EQ(TestObject::destroyed_count, 1);
    }
}

TEST_F(ReferencePtrFixture, SwapExchangesPointers)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TReferencePtr<TestObject> a = MakeReference<TestObject>();
        TReferencePtr<TestObject> b = MakeReference<TestObject>();
        ASSERT_NE(a.get(), nullptr);
        ASSERT_NE(b.get(), nullptr);
        ASSERT_NE(a.get(), b.get());

        auto* rawA = a.get();
        auto* rawB = b.get();

        a.swap(b);
        EXPECT_EQ(a.get(), rawB);
        EXPECT_EQ(b.get(), rawA);
    }
}

// attach / detach：AddRef/Release のバランス確認
TEST_F(ReferencePtrFixture, AttachAndDetach)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        // 手動管理で attach する場合の挙動
        auto* raw = new TestObject(7);

        TReferencePtr<TestObject> p;
        p.attach(raw);           // attach は所有を「受け取る」想定（AddRef しない）
        EXPECT_EQ(p.get(), raw);

        auto* out = p.detach();  // detach は所有を「手放す」
        EXPECT_EQ(out, raw);
        EXPECT_EQ(p.get(), nullptr);

        // 明示 delete されるべき（p 側はもう関知しない）
        delete out;
        EXPECT_EQ(TestObject::destroyed_count, 1);
    }
}

// create：attach ベースのファクトリ
TEST_F(ReferencePtrFixture, CreateUsesAttachSemantics)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        auto* raw = new TestObject(3);
        TReferencePtr<TestObject> p = raw;
        EXPECT_EQ(p.get(), raw);

        // p が残っている間は delete されない
        EXPECT_EQ(TestObject::destroyed_count, 0);

        // p をヌルにすると delete
        p = nullptr;
        EXPECT_EQ(TestObject::destroyed_count, 1);
    }
}

// reset（引数なし）：内部 Release が呼ばれ、破棄をトリガ
TEST_F(ReferencePtrFixture, ResetNoArgReleasesAndReturnsRefcount)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TReferencePtr<TestObject> p = MakeReference<TestObject>();
        ASSERT_NE(p.get(), nullptr);
        // reset() は InternalRelease() の戻り値（Release の戻り値）を返す設計
        // 具体的な数値は実装に依存するため、呼び出し可能であることとヌル化だけを確認
        (void)p.reset();
        EXPECT_EQ(p.get(), nullptr);
        // 破棄発生
        EXPECT_EQ(TestObject::destroyed_count, 1);
    }
}

// reset（引数あり）：入れ替え（古い方を Release、新しい方を保有）
TEST_F(ReferencePtrFixture, ResetWithNewPointer)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        auto* a = new TestObject(1);
        auto* b = new TestObject(2);

        TReferencePtr<TestObject> p;
        p.reset(a); // a を保有
        EXPECT_EQ(p.get(), a);
        EXPECT_EQ(TestObject::destroyed_count, 0);

        p.reset(b); // a を解放し b を保有
        EXPECT_EQ(p.get(), b);
        EXPECT_EQ(TestObject::destroyed_count, 1);

        p = nullptr; // b を解放
        EXPECT_EQ(TestObject::destroyed_count, 2);
    }
}

// get_address_of / release_and_get_address_of：アドレス取得の基本契約
TEST_F(ReferencePtrFixture, AddressHelpers)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
    {
        TReferencePtr<TestObject> p = MakeReference<TestObject>();
        ASSERT_NE(p.get(), nullptr);

        // release_and_get_address_of は先に Release してからアドレスを返す
        auto** slot = p.release_and_get_address_of();
        EXPECT_NE(slot, nullptr);
        EXPECT_EQ(*slot, nullptr);  // Release 後にヌル化されている前提
        EXPECT_EQ(TestObject::destroyed_count, 1);

        // スロット経由で新しいオブジェクトを差し込める（典型的に外部 API が書き込むパターン）
        *slot = new TestObject(10);
        (*slot)->AddRef();

        ASSERT_NE(p.get(), nullptr);
        EXPECT_EQ(p->value, 10);

        p = nullptr; // 後始末
        EXPECT_EQ(TestObject::destroyed_count, 2);
    }
}
}
