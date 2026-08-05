#include "core/window/WindowEventListner.hpp"
#include "core/debug/CompileMessage.hpp"

namespace slug::core
{

WindowEventListner::WindowEventListner()
{
    SLUG_COMPILE_TODO_MESSAGE("Not Thread Safe")
}

WindowCallbackID WindowEventListner::RegisterCallback(const RegisterParam& param)
{
    WindowCallbackID id = {};
    CallbackMapPerEvent& callbackMapPerEvent = m_callbacks[param.targetWindow][param.type];
    if (callbackMapPerEvent.find(id) == callbackMapPerEvent.end())
    {
        callbackMapPerEvent[id] = param.callback;
    }
    return id;
}

void WindowEventListner::UnregisterCallback(WindowCallbackID id)
{
    for (auto& callbackMapPerWindow : m_callbacks)
    {
        for (auto& callbackMapPerEvent : callbackMapPerWindow.second)
        {
            if (callbackMapPerEvent.second.find(id) != callbackMapPerEvent.second.end())
            {
                callbackMapPerEvent.second.erase(id);
            }
        }
    }
}

bool WindowEventListner::HasCallback(WindowID windowID, WindowEventType type) const
{
    auto callbackMapPerWindow = m_callbacks.find(windowID);
    if (callbackMapPerWindow == m_callbacks.end())
    {
        return false;
    }

    auto callbackMapPerEvent = callbackMapPerWindow->second.find(type);
    if (callbackMapPerEvent == callbackMapPerWindow->second.end())
    {
        return false;
    }

    return !callbackMapPerEvent->second.empty();
}

void WindowEventListner::SendEvent(const core::WindowEventBase* event) const
{
    WindowID windowID = event->id;
    WindowEventType type = event->GetType();
    if (m_callbacks.find(windowID) != m_callbacks.end())
    {
        auto& callbacksPerWindow = m_callbacks.find(windowID)->second;
        if (callbacksPerWindow.find(type) != callbacksPerWindow.end())
        {
            auto& callbacksPerEvent = callbacksPerWindow.find(type)->second;
            for (const auto& callback : callbacksPerEvent)
            {
                callback.second(event);
            }
        }
    }
}
}
