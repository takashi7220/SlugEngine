#pragma once

#include "core/memory/SObject.hpp"
#include "core/memory/SharedPtr.hpp"
#include "core/container/String.hpp"
#include "core/Function.hpp"
#include "core/pattern/Closure.hpp"
namespace slug::render
{

struct WindowMessage
{
    uint32_t messageID = 0;
    uint32_t wParam = 0;
    int64_t lParam = 0;
};
using WindowCallback = core::TFunctionObject<void(WindowMessage)>;

struct WindowParam
{
    uint32_t width = 1280;
    uint32_t height = 720;
    core::String name = "SlugWindow";
};

class IWindow : public core::SObject
{
public:
    IWindow() = default;
    virtual ~IWindow() = default;
    virtual bool Create(WindowParam param) = 0;
    virtual void* GetWindowHandle() = 0;
    virtual void SetWindowPos(int32_t x, int32_t y) = 0;
    virtual void GetWindowSize(bool adjustSize, int32_t& x, int32_t& y) = 0;
    virtual bool CheckWindowShouldClose() = 0;
    virtual void Shutdown() = 0;
    virtual void RegistWindowCallback(WindowCallback callback) = 0;
};

class WindowUtility
{
public:
    static core::TReferencePtr<IWindow> CreateWindowsWindow(WindowParam param);
};
}
