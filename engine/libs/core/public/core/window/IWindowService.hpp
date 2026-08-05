#pragma once
#include "core/window/IWindow.hpp"
#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/thread/Mutex.hpp"
#include "core/service/ServiceProvider.hpp"
#include "core/window/WindowEventListner.hpp"

namespace slug::core
{
enum class DpiAwareness
{
    Unaware,
    SystemAware,
    PerMonitorAware
};

enum class WindowPlatformType
{
    Windows,
    Null
};

class IWindowService : public core::IService
{
public:
    struct Desc
    {
        DpiAwareness dpiAwarenes = DpiAwareness::PerMonitorAware;
        WindowPlatformType type = WindowPlatformType::Null;
    };
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IWindowService, core::IService)
    SLUG_INTERFACE_SERVICE_REGISTER(IWindowService)

    IWindowService() = default;
    virtual ~IWindowService() = default;

    virtual bool Initialize(const Desc& desc) = 0;
    virtual void Shutdown() = 0;

    virtual bool PeekMessages(WindowID windowID) = 0;

    virtual WindowPtr RegisterWindow(const IWindow::Desc& desc) = 0;
    virtual bool TryEnumerateMonitors(core::TVector<MonitorInfo>& monitors) const = 0;
    virtual bool TryEnumerateMonitorDetails(core::TVector<MonitorDetailInfo>& monitors) const = 0;
    virtual std::optional<MonitorInfo> GetMonitorFromWindow(UUID id) const = 0;
    virtual std::optional<MonitorInfo> GetPrimaryMonitor() const = 0;

    virtual void SetAppUserAttention(bool flag) = 0;
    virtual void SetClipboardText(core::StringView text) = 0;
    virtual core::String GetClipboardText() const = 0;

    virtual void SetDpiAwareness(DpiAwareness dpi) = 0;
    virtual DpiAwareness GetDpiAwareness() const = 0;

    virtual WindowCallbackID RegisterCallback(const WindowEventListner::RegisterParam& param)
    {
        return m_listner.RegisterCallback(param);
    }

    virtual void UnregisterCallback(WindowCallbackID id)
    {
        m_listner.UnregisterCallback(id);
    }

    virtual bool HasCallback(core::WindowID windowID, WindowEventType type) const
    {
        return m_listner.HasCallback(windowID, type);
    }

    virtual void RequestCloseWindow(core::WindowID id)
    {
        auto itr = m_windows.find(id);
        if (itr != m_windows.end())
        {
            itr->second->Close();
        }
    }

    virtual WindowPtr GetWindow(core::WindowID id)
    {
        auto itr = m_windows.find(id);
        if (itr != m_windows.end())
        {
            return itr->second;
        }
        return nullptr;
    }

    virtual void EnumerateWindows(core::TVector<WindowPtr>& windows) const
    {
        for (auto& itr : m_windows)
        {
            windows.push_back(itr.second);
        }
    }

    virtual size_t GetWindowCount() const
    {
        return m_windows.size();
    }    

    virtual void DestroyWindow(core::WindowID id)
    {
        auto itr = m_windows.find(id);
        if (itr != m_windows.end())
        {
            itr->second->Destroy();
        }
    }

    virtual void UnregisterWindow(core::WindowID id)
    {
        core::ScopedLock lock(m_mutex);
        auto itr = m_windows.find(id);
        if (itr != m_windows.end())
        {
            m_windows.erase(itr);
        }
    }

protected:

    core::Mutex m_mutex;
    core::TUnorderedMap<core::WindowID, WindowPtr, UUIDHash> m_windows = {};
    DpiAwareness m_dpiAwareness = DpiAwareness::PerMonitorAware;
    core::WindowEventListner m_listner;
};

using WindowServicePtr = core::TReferencePtr<IWindowService>;
}
