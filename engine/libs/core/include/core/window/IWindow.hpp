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

/**
 * @brief ウインドウモード
 */
enum class WindowMode
{
    Windowed,               //< ウインドウ
    BorderlessFullscreen,   //< ボーダレスフルスクリーン
    ExclusiveFullscreen     //< 排他フルスクリーン
};

/**
 * @brief カーソルモード
 */
enum class CursorMode
{
    Normal,                 //!< 通常
    Hidden,                 //!< カーソルを非表示
    Disable,                //!< カーソルを無効化
    Confined                //!< ウインドウ外に移動させない
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

struct WindowCloseEvent
{
    UUID id;
};
struct WindowFocusEvent
{
    UUID id;
    bool focused;
};
struct WindowResizeEvent
{
    UUID id;
    int width, height;
    float dpiScale;
};
struct WindowDpiChangedEvent
{
    UUID id;
    float dpiScale;
};
struct WindowMoveEvent
{
    UUID id;
    int x, y;
};
struct WindowShowEvent
{
    UUID id;
    bool shown;
};
struct WindowMinimizeEvent
{
    UUID id;
    bool minimized;
};
struct WindowModeChangedEvent
{
    UUID id;
    WindowMode mode;
    MonitorInfo monitor;
};
struct FileDropEvent
{
    UUID id;
    core::TVector<core::FixedString<260>> paths;
};

using WindowEvent = std::variant<
    WindowCloseEvent,
    WindowFocusEvent,
    WindowResizeEvent,
    WindowDpiChangedEvent,
    WindowMoveEvent,
    WindowShowEvent,
    WindowMinimizeEvent,
    WindowModeChangedEvent,
    FileDropEvent
>;

struct WindowDesc
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
};

struct NativeWindowHandle : public ReferenceObject
{
    virtual void* GetHandle() = 0;
};
using NativeWindowHandlePtr = TReferencePtr<NativeWindowHandle>;

class IWindow : public SObject
{
public:
    virtual ~IWindow() = default;
    virtual StringView GetTitle() const = 0;
    virtual void SetTitle(StringView title) = 0;

    virtual WindowRect ClientRect() const = 0;
    virtual WindowRect WindowRect() const = 0;
    virtual math::Vector2 ContentScale() const = 0;

    virtual bool IsVisible() const = 0;
    virtual bool IsMinimized() const = 0;
    virtual bool IsFocused() const = 0;

    virtual WindowMode GetWindowMode() const = 0;
    virtual void SetWindowMode(WindowMode mode, core::StringView targetName) = 0;

    virtual void Show(bool s) = 0;
    virtual void Minimize(bool m) = 0;
    virtual void Focus() = 0;

    virtual void Resize(int32_t width, int32_t height) = 0;
    virtual void Move(int32_t x, int32_t y) = 0;

    virtual void SetCursorMode(CursorMode mode) = 0;
    virtual void SetCursorVisible(bool flag) = 0;
    virtual void SetCaptionDragEnable(bool flag) = 0;

    using EventCallback = core::TFunctionObject<void(const WindowEvent&)>;
    virtual void SetEventCallback(EventCallback func) = 0;

    using HitTestFunc = core::TFunctionObject<WindowHitResult(math::Vector2 position)>;
    virtual void SetHitTestCallback(HitTestFunc func) = 0;

    virtual void SetFileDropEnabled(bool flag) = 0;

    virtual NativeWindowHandlePtr GetNativeHandle() const = 0;

    using SwapchainRecreateCallback = core::TFunctionObject<void(IWindow&)>;
    virtual void SetSwapchainRecreateCallback(SwapchainRecreateCallback cb) = 0;

    virtual bool GetAllowPresent() const = 0;
    virtual void SetAllowPresent(bool) = 0;

    virtual MonitorInfo GetCurrentMonitorInfo() const = 0;
    virtual MonitorDetailInfo GetCurrentMonitorDetailInfo() const = 0;

protected:
    IWindow() = default;
};
using WindowPtr = TReferencePtr<IWindow>;

}
