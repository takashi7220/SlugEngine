#pragma once

#if defined(PLATFORM_WINDOWS)
#include "core/window/IWindowService.hpp"

namespace slug::core::win
{

class WindowService : public IWindowService
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(WindowService, IWindowService)
    SLUG_INTERFACE_SERVICE_REGISTER(win::WindowService)

    WindowService(const Desc& desc);

    virtual ~WindowService() = default;

    bool Initialize(const Desc& desc) override;
    void Shutdown() override;

    bool PeekMessages(WindowID windowID) override;

    WindowPtr RegisterWindow(const IWindow::Desc& desc) override;

    bool TryEnumerateMonitors(core::TVector<MonitorInfo>& monitors) const override;
    bool TryEnumerateMonitorDetails(core::TVector<MonitorDetailInfo>& monitors) const override;
    std::optional<MonitorInfo> GetMonitorFromWindow(UUID id) const override;
    std::optional<MonitorInfo> GetPrimaryMonitor() const override;

    void SetAppUserAttention(bool flag) override;
    void SetClipboardText(core::StringView text) override;
    core::String GetClipboardText() const override;

    void SetDpiAwareness(DpiAwareness dpi) override;
    DpiAwareness GetDpiAwareness() const override;

    void SendEventListners(const WindowEventBase* event) const;
private:
    bool m_initialized = false;
    static thread_local WindowService* m_threadActive;
};
}
#endif
