#if defined(PLATFORM_WINDOWS)
#include "core/window/win/MonitorUtility.hpp"
#include <windows.h>
#include <shellscalingapi.h>

#pragma comment(lib, "Shcore.lib")

namespace slug::core::win
{

MonitorInfo MonitorUtility::GetMonitorInfomation(HMONITOR targetMonitor)
{
    MONITORINFOEX monitorInfo {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(targetMonitor, &monitorInfo);

    UINT dpiX = 0;
    UINT dpiY = 0;
    GetDpiForMonitor(targetMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

    MonitorInfo ret;
    ret.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    ret.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    ret.name = monitorInfo.szDevice;
    ret.x = monitorInfo.rcMonitor.left;
    ret.y = monitorInfo.rcMonitor.top;
    ret.primary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
    ret.dpiScale = static_cast<float>(dpiX / 96.0f);
    return ret;
}

MonitorDetailInfo MonitorUtility::GetMonitorDetailInfomation(HMONITOR targetMonitor)
{
    MonitorDetailInfo info = GetMonitorInfomation(targetMonitor);
    DEVMODEW dm {};
    dm.dmSize = sizeof(dm);

    for (DWORD i = 0; EnumDisplaySettingsW(StringUtility::GetWstring(info.name.c_str()).c_str(), i, &dm); ++i)
    {
        info.resolutions.push_back(math::Vector2((float)dm.dmPelsWidth, (float)dm.dmPelsHeight));
        info.refreshRates.push_back((float)dm.dmDisplayFrequency);
        info.bitDepths.push_back((float)dm.dmBitsPerPel);
    }
    return info;
}

bool MonitorUtility::TryEnumerateMonitorInfo(core::TVector<MonitorInfo>& outMonitorInfos)
{
    auto enumProc = [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lparam) -> BOOL
    {
        auto* v = reinterpret_cast<core::TVector<MonitorInfo>*>(lparam);
        v->push_back(GetMonitorInfomation(hMonitor));
        return TRUE;
    };

    if (!EnumDisplayMonitors(nullptr, nullptr, enumProc, (LPARAM)&outMonitorInfos))
    {
        return false;
    }

    return true;
}

bool MonitorUtility::TryEnumerateDetailMonitorInfo(core::TVector<MonitorDetailInfo>& outMonitorDetailInfo)
{
    auto enumProc = [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lparam) -> BOOL
        {
            auto* v = reinterpret_cast<core::TVector<MonitorDetailInfo>*>(lparam);
            MonitorDetailInfo& monitorDetailInfo = v->emplace_back();
            monitorDetailInfo = GetMonitorDetailInfomation(hMonitor);
            return TRUE;
        };

    if (!EnumDisplayMonitors(nullptr, nullptr, enumProc, (LPARAM)&outMonitorDetailInfo))
    {
        return false;
    }

    return true;

}

bool MonitorUtility::TryGetMonitorInfo(core::StringView targetName, MonitorInfo& outMonitorInfo)
{
    core::TVector<MonitorInfo> infos = {};
    if (!TryEnumerateMonitorInfo(infos))
    {
        return false;
    }

    for (auto& info : infos)
    {
        if (info.name == targetName)
        {
            outMonitorInfo = info;
            return true;
        }
    }
    return false;
}

bool MonitorUtility::TryGetMonitorDetailInfo(core::StringView targetName, MonitorDetailInfo& outMonitorDetailInfo)
{
    core::TVector<MonitorDetailInfo> infos = {};
    if (!TryEnumerateDetailMonitorInfo(infos))
    {
        return false;
    }

    for (auto& info : infos)
    {
        if (info.name == targetName)
        {
            outMonitorDetailInfo = info;
            return true;
        }
    }
    return false;
}

bool MonitorUtility::TryGetPrimaryMonitorInfo(MonitorInfo& outMonitorInfo)
{
    core::TVector<MonitorInfo> infos;
    if (TryEnumerateMonitorInfo(infos))
    {
        for (auto& info : infos)
        {
            if (info.primary)
            {
                outMonitorInfo = info;
                return true;
            }
        }
    }
    return false;
}

bool MonitorUtility::TryGetPrimaryMonitorDetailInfo(MonitorDetailInfo& outMonitorDetailInfo)
{
    core::TVector<MonitorDetailInfo> infos;
    if (TryEnumerateDetailMonitorInfo(infos))
    {
        for (auto& info : infos)
        {
            if (info.primary)
            {
                outMonitorDetailInfo = info;
                return true;
            }
        }
    }
    return false;
}


}
#endif
