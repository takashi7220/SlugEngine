#pragma once

#if defined(PLATFORM_WINDOWS)

#include <Windows.h>
#include "core/window/IWindow.hpp"
#include "core/container/Vector.hpp"

namespace slug::core::win
{
class MonitorUtility
{
public:
    static MonitorInfo GetMonitorInfomation(HMONITOR targetMonitor);
    static MonitorDetailInfo GetMonitorDetailInfomation(HMONITOR targetMonitor);

    static bool TryEnumerateMonitorInfo(core::TVector<MonitorInfo>& outMonitorInfos);
    static bool TryEnumerateDetailMonitorInfo(core::TVector<MonitorDetailInfo>& outMonitorDetailInfo);
    static bool TryGetMonitorInfo(core::StringView targetName, MonitorInfo& outMonitorInfo);
    static bool TryGetMonitorDetailInfo(core::StringView targetName, MonitorDetailInfo& outMonitorDetailInfo);
    static bool TryGetPrimaryMonitorInfo(MonitorInfo& outMonitorInfo);
    static bool TryGetPrimaryMonitorDetailInfo(MonitorDetailInfo& outMonitorDetailInfo);
};
}
#endif
