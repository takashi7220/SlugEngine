#include <gtest/gtest.h>
#include "core/timer/Timer.hpp"
#include "core/thread/Thread.hpp"

#include <chrono>

using namespace slug::core;

namespace
{
// 実時間に依存するため、下限のみを厳密に見て上限は広めに取る。
void SleepMilliseconds(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
}

//------------------------------------------------------------------------------
// 経過時間
//------------------------------------------------------------------------------
TEST(TimerTest, ElapsedIsNonNegativeImmediatelyAfterConstruction)
{
    Timer timer;

    EXPECT_GE(timer.ElapsedNanoseconds(), 0.0);
    EXPECT_GE(timer.ElapsedMicroseconds(), 0.0);
    EXPECT_GE(timer.ElapsedMilliseconds(), 0.0);
    EXPECT_GE(timer.ElapsedSeconds(), 0.0);
}

TEST(TimerTest, ElapsedIsMonotonicallyIncreasing)
{
    Timer timer;

    const double first = timer.ElapsedNanoseconds();
    SleepMilliseconds(5);
    const double second = timer.ElapsedNanoseconds();

    EXPECT_GT(second, first);
}

TEST(TimerTest, ElapsedUnitsAreConsistent)
{
    Timer timer;
    SleepMilliseconds(20);

    const double ns = timer.ElapsedNanoseconds();
    const double us = timer.ElapsedMicroseconds();
    const double ms = timer.ElapsedMilliseconds();
    const double s = timer.ElapsedSeconds();

    ASSERT_GT(ns, 0.0);

    // 同じ経過時間を単位違いで見ているので、換算すればおおむね一致する。
    EXPECT_NEAR(us, ns / 1000.0, ns / 1000.0 * 0.05 + 1.0);
    EXPECT_NEAR(ms, ns / 1000000.0, ns / 1000000.0 * 0.05 + 1.0);
    EXPECT_NEAR(s, ns / 1000000000.0, ns / 1000000000.0 * 0.05 + 1.0);
}

TEST(TimerTest, ElapsedReflectsSleptDuration)
{
    Timer timer;
    SleepMilliseconds(30);

    // スリープ精度のぶれを見込んで下限のみ確認する。
    EXPECT_GE(timer.ElapsedMilliseconds(), 20.0);
}

//------------------------------------------------------------------------------
// Reset
//------------------------------------------------------------------------------
TEST(TimerTest, ResetRestartsMeasurement)
{
    Timer timer;
    SleepMilliseconds(30);
    const double beforeReset = timer.ElapsedMilliseconds();
    ASSERT_GE(beforeReset, 20.0);

    timer.Reset();
    const double afterReset = timer.ElapsedMilliseconds();

    EXPECT_LT(afterReset, beforeReset);
}

//------------------------------------------------------------------------------
// Lap
//------------------------------------------------------------------------------
TEST(TimerTest, LapMeasuresIntervalSincePreviousLap)
{
    Timer timer;

    SleepMilliseconds(20);
    const double firstLap = timer.LapMilliseconds();
    EXPECT_GE(firstLap, 10.0);

    // 直後のラップは前回ラップからの差分なので、1回目より十分小さい。
    const double secondLap = timer.LapMilliseconds();
    EXPECT_GE(secondLap, 0.0);
    EXPECT_LT(secondLap, firstLap);
}

TEST(TimerTest, LapDoesNotResetTotalElapsed)
{
    Timer timer;

    SleepMilliseconds(20);
    timer.LapMilliseconds();
    SleepMilliseconds(20);

    const double totalElapsed = timer.ElapsedMilliseconds();
    const double lastLap = timer.LapMilliseconds();

    // 累計は2回のスリープ分、ラップは直近1回分。
    EXPECT_GE(totalElapsed, 30.0);
    EXPECT_LT(lastLap, totalElapsed);
}

TEST(TimerTest, LapUnitsAreNonNegative)
{
    Timer timer;
    SleepMilliseconds(5);

    EXPECT_GE(timer.LapNanoseconds(), 0.0);
    EXPECT_GE(timer.LapMicroseconds(), 0.0);
    EXPECT_GE(timer.LapMilliseconds(), 0.0);
    EXPECT_GE(timer.LapSeconds(), 0.0);
}

//------------------------------------------------------------------------------
// 静的な現在時刻
//------------------------------------------------------------------------------
TEST(TimerTest, GetNowIsPositiveAndMonotonic)
{
    const double first = Timer::GetNowNanoseconds();
    EXPECT_GT(first, 0.0);

    SleepMilliseconds(5);
    const double second = Timer::GetNowNanoseconds();

    EXPECT_GT(second, first);
}

TEST(TimerTest, GetNowUnitsAreConsistent)
{
    const double ns = Timer::GetNowNanoseconds();
    const double us = Timer::GetNowMicroseconds();
    const double ms = Timer::GetNowMiliseconds();
    const double s = Timer::GetNowSeconds();

    ASSERT_GT(ns, 0.0);

    // 取得タイミングのずれを許容しつつ、桁が合っていることを確認する。
    EXPECT_NEAR(us, ns / 1000.0, ns / 1000.0 * 0.01);
    EXPECT_NEAR(ms, ns / 1000000.0, ns / 1000000.0 * 0.01);
    EXPECT_NEAR(s, ns / 1000000000.0, ns / 1000000000.0 * 0.01);
}

//------------------------------------------------------------------------------
// コピー・ムーブ
//------------------------------------------------------------------------------
TEST(TimerTest, CopyKeepsSameStartPoint)
{
    Timer timer;
    SleepMilliseconds(20);

    Timer copy(timer);

    // 同じ開始時刻を持つので、経過時間はほぼ一致する。
    EXPECT_NEAR(copy.ElapsedMilliseconds(), timer.ElapsedMilliseconds(), 5.0);
}
