#pragma once

#include "core/window/IWindowService.hpp"
#include "core/thread/Mutex.hpp"
#include "core/window/null/Window.hpp"
#include "core/debug/Assert.hpp"

namespace slug::core::null
{

class WindowService : public IWindowService
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(WindowService, IWindowService)
    SLUG_INTERFACE_SERVICE_REGISTER(null::WindowService)
    WindowService(const Desc& desc)
        : IWindowService()
    {
        Initialize(desc);
    }

    virtual ~WindowService() = default;

    bool Initialize(const Desc& desc) override
    {
        return true;
    }

    void Shutdown() override
    {
    }

    bool PeekMessages(WindowID windowID) override
    {
        return false;
    }

    WindowPtr RegisterWindow(const IWindow::Desc& desc) override
    {
        Window* window = null::Window::Create(desc);
        m_windows[window->GetWindowID()] = window;
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
    Desc m_desc;
};
}
