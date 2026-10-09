#include <gtest/gtest.h>
#include "core/memory/MemoryUtility.hpp"
#include "core/memory/MemoryObject.hpp"
#include "core/memory/Allocator.hpp"
#include "core/container/Vector.hpp"
#include "core/container/String.hpp"

using namespace slug::core;

namespace
{

bool IsAlignedTo(const void* p, size_t align)
{
    return (reinterpret_cast<uintptr_t>(p) % align) == 0;
}

// 確保領域の全バイトに書き込めるかを確認する。
// ヘッダ領域と利用者領域が重なっていると、ここで隣接ブロックを壊す。
void FillAndVerify(void* p, size_t size, uint8_t pattern)
{
    uint8_t* bytes = static_cast<uint8_t*>(p);
    for (size_t i = 0; i < size; ++i)
    {
        bytes[i] = static_cast<uint8_t>(pattern + (i & 0x7F));
    }
    for (size_t i = 0; i < size; ++i)
    {
        ASSERT_EQ(bytes[i], static_cast<uint8_t>(pattern + (i & 0x7F)));
    }
}

struct alignas(64) OverAlignedObject : MemoryObject
{
    double values[8] = {};
};

} // namespace

//------------------------------------------------------------------------------
// Allocate / Deallocate
//------------------------------------------------------------------------------
TEST(MemoryUtilityTest, AllocateReturnsUsableBlock)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        for (size_t size : { size_t(1), size_t(7), size_t(64), size_t(1000), size_t(64 * 1024) })
        {
            void* p = MemoryUtility::Allocate(size);
            ASSERT_NE(p, nullptr);
            FillAndVerify(p, size, 0xA5);
            MemoryUtility::Deallocate(p);
        }
    }
}

TEST(MemoryUtilityTest, DeallocateNullptrIsNoop)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        MemoryUtility::Deallocate(nullptr);
        MemoryUtility::Deallocate(nullptr, 64);
    }
}

// デバッグビルドではヘッダを利用者ポインタの直前に置くため、
// 要求アラインメントを満たすにはヘッダ領域を切り上げる必要がある。
TEST(MemoryUtilityTest, AlignedAllocateSatisfiesRequestedAlignment)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        for (size_t align : { size_t(8), size_t(16), size_t(32), size_t(64), size_t(128), size_t(256) })
        {
            for (size_t size : { size_t(1), size_t(40), size_t(136), size_t(300) })
            {
                void* p = MemoryUtility::Allocate(size, align);
                ASSERT_NE(p, nullptr);
                EXPECT_TRUE(IsAlignedTo(p, align)) << "align=" << align << " size=" << size;
                FillAndVerify(p, size, 0x3C);
                MemoryUtility::Deallocate(p, align);
            }
        }
    }
}

// TAllocator は allocate でアライン版、deallocate で非アライン版を呼ぶ。
// ヘッダがブロック先頭までのオフセットを保持していないとここで崩れる。
TEST(MemoryUtilityTest, AlignedAllocateCanBeFreedByUnalignedDeallocate)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        for (size_t align : { size_t(8), size_t(16) })
        {
            void* p = MemoryUtility::Allocate(128, align);
            ASSERT_NE(p, nullptr);
            FillAndVerify(p, 128, 0x11);
            MemoryUtility::Deallocate(p);
        }
    }
}

TEST(MemoryUtilityTest, MemsetAndMemcpy)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        uint8_t src[16] = {};
        uint8_t dst[16] = {};

        MemoryUtility::Memset(src, 0x7E, sizeof(src));
        for (uint8_t v : src)
        {
            EXPECT_EQ(v, 0x7E);
        }

        MemoryUtility::Memcpy(dst, sizeof(dst), src, sizeof(src));
        EXPECT_EQ(memcmp(src, dst, sizeof(src)), 0);

        MemoryUtility::Memset(dst, 0, sizeof(dst));
        for (uint8_t v : dst)
        {
            EXPECT_EQ(v, 0);
        }
    }
}

//------------------------------------------------------------------------------
// MemoryObject の operator new / delete
//------------------------------------------------------------------------------
TEST(MemoryObjectTest, NewDeleteRoundTrip)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        struct Simple : MemoryObject
        {
            int value = 42;
        };

        Simple* p = new Simple();
        ASSERT_NE(p, nullptr);
        EXPECT_EQ(p->value, 42);
        delete p;
    }
}

// 過剰アラインな型は align 付き operator new が選択される。
// align を捨てていると返却ポインタが要求境界を満たさない。
TEST(MemoryObjectTest, OverAlignedTypeHonorsAlignment)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        for (int i = 0; i < 8; ++i)
        {
            OverAlignedObject* p = new OverAlignedObject();
            ASSERT_NE(p, nullptr);
            EXPECT_TRUE(IsAlignedTo(p, alignof(OverAlignedObject)));
            p->values[7] = 1.0;
            EXPECT_DOUBLE_EQ(p->values[7], 1.0);
            delete p;
        }
    }
}

TEST(MemoryObjectTest, NewObjectForwardsConstructorArguments)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        struct WithArgs : MemoryObject
        {
            WithArgs(int a, double b)
                : first(a)
                , second(b)
            {
            }

            int first;
            double second;
        };

        WithArgs* p = NewObject<WithArgs>(7, 2.5);
        ASSERT_NE(p, nullptr);
        EXPECT_EQ(p->first, 7);
        EXPECT_DOUBLE_EQ(p->second, 2.5);
        delete p;
    }
}

//------------------------------------------------------------------------------
// TAllocator
//------------------------------------------------------------------------------
TEST(AllocatorTest, AllocateDeallocateRoundTrip)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TAllocator<uint32_t> allocator;

        uint32_t* p = allocator.allocate(64);
        ASSERT_NE(p, nullptr);
        EXPECT_TRUE(IsAlignedTo(p, alignof(uint32_t)));

        for (size_t i = 0; i < 64; ++i)
        {
            p[i] = static_cast<uint32_t>(i);
        }
        for (size_t i = 0; i < 64; ++i)
        {
            EXPECT_EQ(p[i], static_cast<uint32_t>(i));
        }

        allocator.deallocate(p, 64);
    }
}

TEST(AllocatorTest, CheckAllocatableRejectsOverflowingSize)
{
    TAllocator<uint64_t> allocator;
    EXPECT_TRUE(allocator.CheckAllocatable(1024));
    EXPECT_FALSE(allocator.CheckAllocatable(ULLONG_MAX));
}

TEST(AllocatorTest, ContainersUsingAllocatorSurviveGrowth)
{
    SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
    {
        TVector<String> values;
        for (int i = 0; i < 256; ++i)
        {
            values.push_back(String("item-") + String(std::to_string(i).c_str()));
        }

        ASSERT_EQ(values.size(), 256u);
        EXPECT_EQ(values.front(), "item-0");
        EXPECT_EQ(values.back(), "item-255");

        values.clear();
        values.shrink_to_fit();
        EXPECT_TRUE(values.empty());
    }
}

//------------------------------------------------------------------------------
// MemoryLabel / ラベルスコープ
//------------------------------------------------------------------------------
TEST(MemoryLabelTest, EqualityComparesName)
{
    MemoryLabel a("Render");
    MemoryLabel b("Render");
    MemoryLabel c("Audio");

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(MemoryLabelTest, DefaultConstructedLabelIsEmpty)
{
    MemoryLabel label;
    EXPECT_STREQ(label.name, "");
}

TEST(MemoryLabelTest, LongNameIsTruncatedWithinBuffer)
{
    const slug::core::String longName(SLUG_MEMORY_LABEL_LENGTH * 2, 'x');
    MemoryLabel label(longName.c_str());

    EXPECT_LT(strlen(label.name), static_cast<size_t>(SLUG_MEMORY_LABEL_LENGTH));
}

#if SLUG_ENABLE_MEMORY_COUNTER

TEST(MemoryLabelScopeTest, CurrentIsRestoredOnScopeExit)
{
    const MemoryLabel outer = MemoryLabelScope::Current();
    {
        MemoryLabelScope scope("ScopeLevel1");
        EXPECT_STREQ(MemoryLabelScope::Current().name, "ScopeLevel1");
        {
            MemoryLabelScope nested("ScopeLevel2");
            EXPECT_STREQ(MemoryLabelScope::Current().name, "ScopeLevel2");
        }
        EXPECT_STREQ(MemoryLabelScope::Current().name, "ScopeLevel1");
    }
    EXPECT_STREQ(MemoryLabelScope::Current().name, outer.name);
}

TEST(MemoryLabelScopeTest, SubLabelCurrentIsRestoredOnScopeExit)
{
    const MemoryLabel outer = MemorySubLabelScope::Current();
    {
        MemorySubLabelScope scope("SubScope");
        EXPECT_STREQ(MemorySubLabelScope::Current().name, "SubScope");
    }
    EXPECT_STREQ(MemorySubLabelScope::Current().name, outer.name);
}

TEST(MemoryLabelScopeTest, MacroScopeSetsLabel)
{
    {
        SLUG_MEMORY_LABEL_SCOPE(UnitTestLabel)
        EXPECT_STREQ(MemoryLabelScope::Current().name, "UnitTestLabel");
    }
}

//------------------------------------------------------------------------------
// MemoryCounter
//------------------------------------------------------------------------------
TEST(MemoryCounterTest, IncrementAndDecrementTracksPerLabel)
{
    MemoryCounter counter;
    const MemoryLabel label("CounterLabel");
    const MemoryLabel subLabel("CounterSub");
    const MemoryLabel otherSubLabel("OtherSub");

    EXPECT_EQ(counter.GetCurrentMemorySize(label, subLabel), 0u);

    counter.IncrementMemorySize(256, label, subLabel);
    EXPECT_EQ(counter.GetCurrentMemorySize(label, subLabel), 256u);

    counter.IncrementMemorySize(128, label, subLabel);
    EXPECT_EQ(counter.GetCurrentMemorySize(label, subLabel), 384u);

    // サブラベルが違えば別枠で集計される。
    counter.IncrementMemorySize(64, label, otherSubLabel);
    EXPECT_EQ(counter.GetCurrentMemorySize(label, subLabel), 384u);
    EXPECT_EQ(counter.GetCurrentMemorySize(label, otherSubLabel), 64u);

    counter.DecrementMemorySize(384, label, subLabel);
    counter.DecrementMemorySize(64, label, otherSubLabel);
    EXPECT_EQ(counter.GetCurrentMemorySize(label, subLabel), 0u);
    EXPECT_EQ(counter.GetCurrentTotalMemorySize(), 0u);
}

TEST(MemoryCounterTest, DecrementZeroIsIgnored)
{
    MemoryCounter counter;
    const MemoryLabel label("ZeroLabel");
    const MemoryLabel subLabel("ZeroSub");

    counter.DecrementMemorySize(0, label, subLabel);
    EXPECT_EQ(counter.GetCurrentMemorySize(label, subLabel), 0u);
}

TEST(MemoryCounterTest, UnknownLabelReportsZero)
{
    MemoryCounter counter;
    EXPECT_EQ(counter.GetCurrentMemorySize(MemoryLabel("Nope"), MemoryLabel("Nope")), 0u);
    EXPECT_EQ(counter.GetCurrentTotalMemorySize(), 0u);
}

// 確保と解放が同じラベルに同じ量を積み降ろしすることを、実際のアロケートで確認する。
// カウンタはプロセス全体で共有されているため差し替えず、専用ラベルの増減だけを見る。
TEST(MemoryCounterTest, AllocationIsAttributedToCurrentLabel)
{
    const MemoryLabel label("AttributedLabel");
    const MemoryLabel subLabel("AttributedSub");
    const size_t before = MemoryDebugUtility::GetCurrentMemorySize(label, subLabel);
    {
        MemoryLabelScope labelScope(label);
        MemorySubLabelScope subLabelScope(subLabel);

        void* p = MemoryUtility::Allocate(1024);
        ASSERT_NE(p, nullptr);
        EXPECT_GE(MemoryDebugUtility::GetCurrentMemorySize(label, subLabel), before + 1024u);
        MemoryUtility::Deallocate(p);
    }
    EXPECT_EQ(MemoryDebugUtility::GetCurrentMemorySize(label, subLabel), before);
}

// ヘッダ分を含めて確保時と解放時で同じ量が記録されること。
// 記録する大きさがずれていると、ここでラベルの残量が 0 に戻らない。
TEST(MemoryCounterTest, AlignedAllocationIsBalanced)
{
    const MemoryLabel label("AlignedLabel");
    const MemoryLabel subLabel("AlignedSub");
    const size_t before = MemoryDebugUtility::GetCurrentMemorySize(label, subLabel);
    {
        MemoryLabelScope labelScope(label);
        MemorySubLabelScope subLabelScope(subLabel);

        for (size_t align : { size_t(8), size_t(16), size_t(64), size_t(256) })
        {
            void* p = MemoryUtility::Allocate(512, align);
            ASSERT_NE(p, nullptr);
            EXPECT_GT(MemoryDebugUtility::GetCurrentMemorySize(label, subLabel), before);
            MemoryUtility::Deallocate(p, align);
            EXPECT_EQ(MemoryDebugUtility::GetCurrentMemorySize(label, subLabel), before);
        }
    }
}

// 非アライン解放で返した場合も記録が釣り合うこと。
TEST(MemoryCounterTest, AlignedAllocateUnalignedDeallocateIsBalanced)
{
    const MemoryLabel label("MixedFreeLabel");
    const MemoryLabel subLabel("MixedFreeSub");
    const size_t before = MemoryDebugUtility::GetCurrentMemorySize(label, subLabel);
    {
        MemoryLabelScope labelScope(label);
        MemorySubLabelScope subLabelScope(subLabel);

        void* p = MemoryUtility::Allocate(256, 16);
        ASSERT_NE(p, nullptr);
        MemoryUtility::Deallocate(p);
    }
    EXPECT_EQ(MemoryDebugUtility::GetCurrentMemorySize(label, subLabel), before);
}

TEST(MemoryDebugUtilityTest, CounterIsBoundByTestHarness)
{
    EXPECT_NE(MemoryDebugUtility::GetMemoryCounter(), nullptr);
}

#endif // SLUG_ENABLE_MEMORY_COUNTER
