#pragma once

#include <cstdint>

namespace slug::core
{

class Timer
{
public:
    Timer();
    ~Timer();

    Timer(const Timer&) = default;
    Timer& operator=(const Timer&) = default;

    Timer(Timer&&) noexcept = default;
    Timer& operator=(Timer&&) noexcept = default;

    void Reset();

    double ElapsedSeconds() const;
    double ElapsedMilliseconds() const;
    double ElapsedMicroseconds() const;
    double ElapsedNanoseconds() const;

    double LapSeconds();
    double LapMilliseconds();
    double LapMicroseconds();
    double LapNanoseconds();

    static double GetNowSeconds();
    static double GetNowMiliseconds();
    static double GetNowMicroseconds();
    static double GetNowNanoseconds();

private:
    int64_t m_startNanoseconds = 0;
    int64_t m_lastNanoseconds = 0;
};
}
