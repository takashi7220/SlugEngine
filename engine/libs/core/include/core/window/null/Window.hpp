#pragma once

#include "core/window/IWindow.hpp"
#include <Windows.h>

namespace slug::core::null
{

struct NullWindowHandle final: public NativeWindowHandle
{
    void* GetHandle() override
    {
        return nullptr;
    }

};

class Window final : public IWindow
{
public:
    static Window* Create(const WindowDesc& desc)
    {
        Window* w = new Window();
        w->m_desc = desc;
        return w;
    }

    explicit Window() = default;
    ~Window() = default;

    StringView GetTitle() const override
    {
        return m_desc.title.c_str();
    }

    void SetTitle(StringView title)
    {
        m_desc.title = title.data();
    }

    core::WindowRect ClientRect() const override
    {
        return core::WindowRect();
    }

    core::WindowRect WindowRect() const override
    {
        return core::WindowRect();        
    }

    math::Vector2 ContentScale() const override
    {
        return math::Vector2::Zero();
    }

    bool IsVisible() const override
    {
        return false;
    }

    bool IsMinimized() const override
    {
        return false;
    }

    bool IsFocused() const override
    {
        return false;
    }

    WindowMode GetWindowMode() const override
    {
        return WindowMode::Windowed;
    }

    void SetWindowMode(WindowMode mode, core::StringView targetName) override {}

    void Show(bool s) override {}

    void Minimize(bool m) override {}
    void Focus() override {}

    void Resize(int32_t width, int32_t height) override{}
    void Move(int32_t x, int32_t y) override {}

    void SetCursorMode(CursorMode mode) override {}
    void SetCursorVisible(bool flag) override {}
    void SetCaptionDragEnable(bool flag) override {}

    void SetEventCallback(EventCallback event) override {}

    void SetHitTestCallback(HitTestFunc func) override {}

    
    void SetFileDropEnabled(bool flag) override{}

    NativeWindowHandlePtr GetNativeHandle() const override
    {
        return new NullWindowHandle();
    }

    void SetSwapchainRecreateCallback(SwapchainRecreateCallback cb) override {}

    bool GetAllowPresent() const override
    {
        return false;
    }

    void SetAllowPresent(bool) override {}

    MonitorInfo GetCurrentMonitorInfo() const override
    {
        return MonitorInfo();
    }

    MonitorDetailInfo GetCurrentMonitorDetailInfo() const override
    {
        return MonitorDetailInfo();
    }

private:
    WindowDesc m_desc;
};

}

