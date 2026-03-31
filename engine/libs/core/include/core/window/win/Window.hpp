#pragma once
#if defined(PLATFORM_WINDOWS)

#include "core/window/IWindow.hpp"
#include <Windows.h>

namespace slug::core::win
{

struct WinWindowHandle final: public NativeWindowHandle
{
    HWND m_windowHandle;
    void* GetHandle() override
    {
        return (void*)m_windowHandle;
    }

};

class Window final : public IWindow
{
public:
    static Window* Create(const WindowDesc& desc);
    explicit Window();
    ~Window();

    StringView GetTitle() const override;
    void SetTitle(StringView title);

    core::WindowRect ClientRect() const override;
    core::WindowRect WindowRect() const override;
    math::Vector2 ContentScale() const override;

    bool IsVisible() const override;
    bool IsMinimized() const override;
    bool IsFocused() const override;

    WindowMode GetWindowMode() const override;
    void SetWindowMode(WindowMode mode, core::StringView targetName) override;

    void Show(bool s) override;
    void Minimize(bool m) override;
    void Focus() override;

    void Resize(int32_t width, int32_t height) override;
    void Move(int32_t x, int32_t y) override;

    void SetCursorMode(CursorMode mode) override;
    void SetCursorVisible(bool flag) override;
    void SetCaptionDragEnable(bool flag) override;

    void SetEventCallback(EventCallback event) override;

    void SetHitTestCallback(HitTestFunc func) override;

    
    void SetFileDropEnabled(bool flag) override;

    NativeWindowHandlePtr GetNativeHandle() const override;

    void SetSwapchainRecreateCallback(SwapchainRecreateCallback cb) override;

    bool GetAllowPresent() const override;
    void SetAllowPresent(bool) override;

    MonitorInfo GetCurrentMonitorInfo() const override;
    MonitorDetailInfo GetCurrentMonitorDetailInfo() const override;


    void RequestSwapchainRecreate();

    void HandleMessage(UINT msg, WPARAM wp, LPARAM lp, LRESULT& outResult, bool& handled);

private:
    void CreateWindowInternal(const WindowDesc& desc);
    void ApplyBorderless(const MonitorInfo& info);
    void RestoreWindowedFromBorderless();

    float QueryDpiScale() const;

    HMONITOR GetCurrentMonitor() const;

    void UpdateCursorClip();

    bool SetWindowModeInternal(WindowMode mode, const MonitorInfo& info);

private:
    HWND m_hwnd = nullptr;
    DWORD m_style = 0;
    DWORD m_exStyle = 0;
    bool m_shown = false;
    bool m_decorated = true;
    bool m_resizable = true;
    bool m_captionDragEnabled = false;
    bool m_fileDropEnabled = false;
    bool m_allowPresent = false;
    core::WindowMode m_mode = core::WindowMode::Windowed;

    RECT m_windowedRect {};
    HMONITOR m_windowedMonitor = nullptr;

    int32_t m_desiredWidth = 1280;
    int32_t m_desiredHeight = 720;

    EventCallback m_eventCallback = nullptr;

    HitTestFunc m_hitTestCallback = nullptr;
    core::WindowRect m_rect { 0,0,0,0 };
    SwapchainRecreateCallback m_swapchainRecreateCallback = nullptr;

    CursorMode m_cursorMode = CursorMode::Normal;
    bool m_cursorVisible = true;
};

}

#endif
