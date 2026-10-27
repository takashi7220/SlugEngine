#pragma once

#include "core/window/IWindow.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/thread/Mutex.hpp"

namespace slug::core
{
using WindowCallbackID = RuntimeID;

class WindowEventListner
{
public:
    struct RegisterParam
    {
        WindowID targetWindow = {};
        bool hookAllWindow = false;
        WindowEventType type = WindowEventType::Unknown;
        IWindow::EventCallback callback = nullptr;
    };
    WindowEventListner();

    WindowCallbackID RegisterCallback(const RegisterParam& param);
    void UnregisterCallback(WindowCallbackID id);
    bool HasCallback(WindowID windowID, WindowEventType type) const;
    void SendEvent(const core::WindowEventBase* event) const;

private:
    using CallbackMapPerEvent = core::TUnorderedMap<WindowCallbackID, IWindow::EventCallback, RuntimeIDHash>;
    using CallbackMapPerWindow = core::TUnorderedMap<WindowEventType, CallbackMapPerEvent>;

private:
    core::TUnorderedMap<WindowID, CallbackMapPerWindow, RuntimeIDHash> m_callbacks;
};
}
