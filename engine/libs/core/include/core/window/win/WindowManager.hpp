#pragma once

#if defined(PLATFORM_WINDOWS)
#include "core/window/IWindowManager.hpp"

namespace slug::core::win
{

class WindowManager : public IWindowManager
{
public:
    SLUG_INTERFACE_SERVICE_REGISTER(win::WindowManager)

    WindowManager(const WindowManagerDesc& desc);

    virtual ~WindowManager() = default;

    bool Initialize(const WindowManagerDesc& desc) override;
    void Shutdown() override;

    void PumpOSMessages() override;

    WindowPtr AddWindow(const WindowDesc& desc) override;

    bool TryEnumerateMonitors(core::TVector<MonitorInfo>& monitors) const override;
    bool TryEnumerateMonitorDetails(core::TVector<MonitorDetailInfo>& monitors) const override;
    std::optional<MonitorInfo> GetMonitorFromWindow(UUID id) const override;
    std::optional<MonitorInfo> GetPrimaryMonitor() const override;

    void SetAppUserAttention(bool flag) override;
    void SetClipboardText(core::StringView text) override;
    core::String GetClipboardText() const override;

    void SetDpiAwareness(DpiAwareness dpi) override;
    DpiAwareness GetDpiAwareness() const override;

    void SendEventListners(const WindowEvent& event) const;
private:
    bool m_initialized = false;
    static thread_local WindowManager* m_threadActive;
};
}
#endif
