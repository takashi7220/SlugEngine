#include "core/timer/Timer.hpp"

#include <chrono>

namespace
{
using Clock = std::chrono::steady_clock;

int64_t GetNowNanoseconds()
{
    const auto now = Clock::now();
    const auto duration = now.time_since_epoch();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

double NanosecondsToSeconds(int64_t nanoseconds)
{
    return static_cast<double>(nanoseconds) / 1'000'000'000.0;
}

double NanosecondsToMilliseconds(int64_t nanoseconds)
{
    return static_cast<double>(nanoseconds) / 1'000'000.0;
}

double NanosecondsToMicroseconds(int64_t nanoseconds)
{
    return static_cast<double>(nanoseconds) / 1'000.0;
}

double NanosecondsToNanoseconds(int64_t nanoseconds)
{
    return static_cast<double>(nanoseconds);
}
}

namespace slug::core
{
Timer::Timer()
{
    Reset();
}

Timer::~Timer() = default;

void Timer::Reset()
{
    const int64_t now = GetNowNanoseconds();

    m_startNanoseconds = now;
    m_lastNanoseconds = now;
}

double Timer::ElapsedSeconds() const
{
    return NanosecondsToSeconds(GetNowNanoseconds() - m_startNanoseconds);
}

double Timer::ElapsedMilliseconds() const
{
    return NanosecondsToMilliseconds(GetNowNanoseconds() - m_startNanoseconds);
}

double Timer::ElapsedMicroseconds() const
{
    return NanosecondsToMicroseconds(GetNowNanoseconds() - m_startNanoseconds);
}

double Timer::ElapsedNanoseconds() const
{
    return NanosecondsToNanoseconds(GetNowNanoseconds() - m_startNanoseconds);
}

double Timer::LapSeconds()
{
    const int64_t now = GetNowNanoseconds();
    const int64_t elapsed = now - m_lastNanoseconds;

    m_lastNanoseconds = now;

    return NanosecondsToSeconds(elapsed);
}

double Timer::LapMilliseconds()
{
    const int64_t now = GetNowNanoseconds();
    const int64_t elapsed = now - m_lastNanoseconds;

    m_lastNanoseconds = now;

    return NanosecondsToMilliseconds(elapsed);
}

double Timer::LapMicroseconds()
{
    const int64_t now = GetNowNanoseconds();
    const int64_t elapsed = now - m_lastNanoseconds;

    m_lastNanoseconds = now;

    return NanosecondsToMicroseconds(elapsed);
}

double Timer::LapNanoseconds()
{
    const int64_t now = GetNowNanoseconds();
    const int64_t elapsed = now - m_lastNanoseconds;

    m_lastNanoseconds = now;

    return NanosecondsToNanoseconds(elapsed);
}
}
