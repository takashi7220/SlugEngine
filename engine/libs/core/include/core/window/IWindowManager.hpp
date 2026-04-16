#pragma once
#include "core/window/IWindow.hpp"
#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/thread/Mutex.hpp"
#include "core/service/ServiceProvider.hpp"

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

struct WindowManagerDesc
{
    DpiAwareness dpiAwarenes = DpiAwareness::PerMonitorAware;
    WindowPlatformType type = WindowPlatformType::Null;
};

class IWindowManager : public core::IService
{
public:
    SLUG_INTERFACE_SERVICE_REGISTER(IWindowManager)

    IWindowManager() = default;
    virtual ~IWindowManager() = default;

    virtual bool Initialize(const WindowManagerDesc& desc) = 0;
    virtual void Shutdown() = 0;

    virtual void PumpOSMessages() = 0;

    virtual WindowPtr AddWindow(const WindowDesc& desc) = 0;
    virtual bool TryEnumerateMonitors(core::TVector<MonitorInfo>& monitors) const = 0;
    virtual bool TryEnumerateMonitorDetails(core::TVector<MonitorDetailInfo>& monitors) const = 0;
    virtual std::optional<MonitorInfo> GetMonitorFromWindow(UUID id) const = 0;
    virtual std::optional<MonitorInfo> GetPrimaryMonitor() const = 0;

    virtual void SetAppUserAttention(bool flag) = 0;
    virtual void SetClipboardText(core::StringView text) = 0;
    virtual core::String GetClipboardText() const = 0;

    virtual void SetDpiAwareness(DpiAwareness dpi) = 0;
    virtual DpiAwareness GetDpiAwareness() const = 0;

    virtual void DestroyWindow(UUID id)
    {
        LockGuard<Mutex> lock(m_mutex);
        auto itr = m_windows.find(id);
        if (itr != m_windows.end())
        {
            itr->second.reset();
            m_windows.erase(itr);
        }
    }

    virtual WindowPtr GetWindow(UUID id)
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

    using ListenerId = uint64_t;
    virtual ListenerId AddEventListener(IWindow::EventCallback cb)
    {
        LockGuard<Mutex> lock(m_mutex);
        ListenerId id = m_nextListenerId++;
        m_listeners[id] = std::move(cb);
        return id;
    }

    virtual void RemoveEventListener(ListenerId id)
    {
        LockGuard<Mutex> lock(m_mutex);
        auto itr = m_listeners.find(id);
        if (itr != m_listeners.end())
        {
            m_listeners.erase(itr);
        }
    }

protected:
    core::Mutex m_mutex;
    core::TUnorderedMap<UUID, WindowPtr, UUIDHash> m_windows = {};
    core::TUnorderedMap<ListenerId, IWindow::EventCallback> m_listeners;
    ListenerId m_nextListenerId = 1;
    DpiAwareness m_dpiAwareness = DpiAwareness::PerMonitorAware;
};

using WindowManagerPtr = core::TReferencePtr<IWindowManager>;
}
