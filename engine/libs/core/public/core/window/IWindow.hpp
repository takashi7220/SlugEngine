#pragma once
#include <optional>
#include "math/Vector2.hpp"
#include "core/object/SObject.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/Function.hpp"
#include <variant>

namespace slug::core
{
using WindowID = core::UUID;

enum class WindowMode
{
    Windowed,
    BorderlessFullscreen,
    ExclusiveFullscreen
};

enum class CursorMode
{
    Normal,
    Hidden,
    Disable,
    Confined
};

enum class WindowHitResult
{
    Normal,
    Client,
    Caption,
    Left,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
};

struct MonitorInfo
{
    FixedString<64> name;
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    float dpiScale = 1.0f;
    bool primary = false;

    MonitorInfo() = default;
    MonitorInfo(const MonitorInfo&) = default;
    MonitorInfo& operator=(const MonitorInfo&) = default;
};

struct MonitorDetailInfo : public MonitorInfo
{
    core::TVector<math::Vector2> resolutions;
    core::TVector<float> refreshRates;
    core::TVector<float> bitDepths;

    MonitorDetailInfo() = default;
    MonitorDetailInfo(const MonitorInfo& info)
        : MonitorInfo(info)
    {
    }
};

struct WindowRect
{
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;
};

enum class WindowEventType : uint8_t
{
    Unknown,
    WindowClose,
    WindowFocus,
    WindowResize,
    WindowDpiChanged,
    WindowMove,
    WindowShow,
    WindowMinimize,
    WindowModeChanged,
    FileDrop,
    WindowDestroy
};

struct WindowEventBase
{
    WindowID id;

    WindowEventBase(WindowID _id)
        : id(_id)
    {
    }

    virtual ~WindowEventBase() = default;

    virtual WindowEventType GetType() const
    {
        return WindowEventType::Unknown;
    }
};

struct WindowCloseEvent : public WindowEventBase
{
    WindowCloseEvent(WindowID _id)
        : WindowEventBase(_id)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowClose;
    }
};

struct WindowFocusEvent : public WindowEventBase
{
    bool focused;

    WindowFocusEvent(WindowID _id, bool _focused)
        : WindowEventBase(_id)
        , focused(_focused)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowFocus;
    }
};

struct WindowResizeEvent : public WindowEventBase
{
    int width, height;
    float dpiScale;

    WindowResizeEvent(WindowID _id, int _width, int _height, float _dpiScale)
        : WindowEventBase(_id)
        , width(_width)
        , height(_height)
        , dpiScale(_dpiScale)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowResize;
    }
};

struct WindowDpiChangedEvent : public WindowEventBase
{
    float dpiScale;

    WindowDpiChangedEvent(WindowID _id, float _dpiScale)
        : WindowEventBase(_id)
        , dpiScale(_dpiScale)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowDpiChanged;
    }
};

struct WindowMoveEvent : public WindowEventBase
{
    int x, y;

    WindowMoveEvent(WindowID _id, int _x, int _y)
        : WindowEventBase(_id)
        , x(_x)
        , y(_y)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowMove;
    }
};

struct WindowShowEvent : public WindowEventBase
{
    bool shown;

    WindowShowEvent(WindowID _id, bool _shown)
        : WindowEventBase(_id)
        , shown(_shown)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowShow;
    }
};

struct WindowMinimizeEvent : public WindowEventBase
{
    bool minimized;

    WindowMinimizeEvent(WindowID _id, bool _minimized)
        : WindowEventBase(_id)
        , minimized(_minimized)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowMinimize;
    }
};

struct WindowModeChangedEvent : public WindowEventBase
{
    WindowMode mode;
    MonitorInfo monitor;

    WindowModeChangedEvent(WindowID _id, WindowMode _mode, const MonitorInfo& _monitor)
        : WindowEventBase(_id)
        , mode(_mode)
        , monitor(_monitor)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowModeChanged;
    }
};

struct WindowFileDropEvent : public WindowEventBase
{
    WindowID id;
    core::TVector<core::FixedString<260>> paths;

    WindowFileDropEvent(WindowID _id, const core::TVector<core::FixedString<260>>& _paths)
        : WindowEventBase(_id)
        , paths(_paths)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::FileDrop;
    }
};

struct WindowDestroyEvent : public WindowEventBase
{
    WindowDestroyEvent(WindowID _id)
        : WindowEventBase(_id)
    {
    }

    WindowEventType GetType() const override
    {
        return WindowEventType::WindowDestroy;
    }
};

struct NativeWindowHandle : public ReferenceObject
{
    virtual void* GetHandle() = 0;
};
using NativeWindowHandlePtr = TReferencePtr<NativeWindowHandle>;


class IWindow : public core::SObject
{
public:
    struct Desc
    {
        core::FixedString<32> title = "Default";
        int32_t width = 1600;
        int32_t height = 600;
        WindowMode mode = WindowMode::Windowed;
        bool resizable = true;
        bool decorated = true;
        bool transparent = false;
        bool alwaysOnTop = false;
        core::FixedString<128> initialMonitor = "";
        std::optional<math::Vector2> position = {};
        float contentScale = 0.0f;
        bool autoDestroyOnClose = true;
    };

    using EventCallback = core::TFunctionObject<void(const WindowEventBase*)>;
    using HitTestFunc = core::TFunctionObject<WindowHitResult(math::Vector2 position)>;
    using SwapchainRecreateCallback = core::TFunctionObject<void(IWindow&)>;

public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IWindow, core::SObject)

    virtual ~IWindow() = default;
    virtual String GetTitle() const = 0;
    virtual void SetTitle(StringView title) = 0;

    virtual WindowRect ClientRect() const = 0;
    virtual WindowRect WindowRect() const = 0;
    virtual math::Vector2 ContentScale() const = 0;

    virtual uint32_t GetWidth()
    {
        return ClientRect().w;
    }

    virtual uint32_t GetHeight()
    {
        return ClientRect().h;
    }

    virtual bool IsVisible() const = 0;
    virtual bool IsMinimized() const = 0;
    virtual bool IsFocused() const = 0;

    virtual WindowMode GetWindowMode() const = 0;
    virtual void SetWindowMode(WindowMode mode, core::StringView targetName) = 0;

    virtual void Show(bool s) = 0;
    virtual void Minimize(bool m) = 0;
    virtual void Focus() = 0;
    virtual void Close() = 0;
    virtual void Destroy() = 0;

    virtual void Resize(int32_t width, int32_t height) = 0;
    virtual void Move(int32_t x, int32_t y) = 0;

    virtual void SetCursorMode(CursorMode mode) = 0;
    virtual void SetCursorVisible(bool flag) = 0;
    virtual void SetCaptionDragEnable(bool flag) = 0;

    virtual void SetEventCallback(EventCallback func) = 0;

    virtual void SetHitTestCallback(HitTestFunc func) = 0;

    virtual void SetFileDropEnabled(bool flag) = 0;

    virtual NativeWindowHandlePtr GetNativeHandle() const = 0;

    virtual void SetSwapchainRecreateCallback(SwapchainRecreateCallback cb) = 0;

    virtual bool GetAllowPresent() const = 0;
    virtual void SetAllowPresent(bool) = 0;

    virtual MonitorInfo GetCurrentMonitorInfo() const = 0;
    virtual MonitorDetailInfo GetCurrentMonitorDetailInfo() const = 0;

    void SetName(core::StringView str)
    {
        m_name = str;
    }

    core::StringView GetName()
    {
        return m_name.data();
    }

    const WindowID& GetWindowID()
    {
        return m_uuid;
    }

protected:
    core::WindowID m_uuid;
    core::FixedString<128> m_name;
    IWindow() = default;
};
using WindowPtr = TReferencePtr<IWindow>;

}
