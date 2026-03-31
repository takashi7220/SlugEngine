#pragma once

#include "core/window/IWindowManager.hpp"
#include "core/thread/Mutex.hpp"
#include "core/window/null/Window.hpp"
#include "core/debug/Assert.hpp"

namespace slug::core::null
{

class WindowManager : public IWindowManager
{
public:
    SLUG_INTERFACE_SERVICE_REGISTER(null::WindowManager)
    WindowManager(const WindowManagerDesc& desc)
        : IWindowManager()
    {
        Initialize(desc);
    }

    virtual ~WindowManager() = default;

    bool Initialize(const WindowManagerDesc& desc) override
    {
        return true;
    }

    void Shutdown() override
    {
    }

    void PumpOSMessages() override
    {
    }

    WindowPtr AddWindow(const WindowDesc& desc) override
    {
        Window* window = null::Window::Create(desc);
        m_windows[window->GetUUID()] = window;
        return window;
    }

    bool TryEnumerateMonitors(core::TVector<MonitorInfo>& monitors) const override
    {
        return false;
    }

    bool TryEnumerateMonitorDetails(core::TVector<MonitorDetailInfo>& monitors) const override
    {
        return false;
    }

    std::optional<MonitorInfo> GetMonitorFromWindow(UUID id) const override
    {
        return std::nullopt;
    }

    std::optional<MonitorInfo> GetPrimaryMonitor() const override
    {
        return std::nullopt;
    }

    void SetAppUserAttention(bool flag) override
    {
    }

    void SetClipboardText(core::StringView text) override
    {
    }

    core::String GetClipboardText() const override
    {
        return "null";
    }

    void SetDpiAwareness(DpiAwareness dpi) override
    {

    }

    DpiAwareness GetDpiAwareness() const override
    {
        return DpiAwareness::PerMonitorAware;
    }

private:
    WindowManagerDesc m_desc;
};
}
