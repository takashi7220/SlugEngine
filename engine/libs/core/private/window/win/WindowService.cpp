#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include <shellapi.h>

#include "core/debug/CompileMessage.hpp"
#include "core/window/win/Window.hpp"
#include "core/window/win/WindowService.hpp"
#include "core/window/win/MonitorUtility.hpp"

namespace slug::core::win
{

thread_local WindowService* WindowService::m_threadActive = nullptr;

WindowService::WindowService(const Desc& desc)
    : IWindowService()
{
    this->Initialize(desc);
}

bool WindowService::Initialize(const Desc& desc)
{
    m_threadActive = this;
    SetDpiAwareness(desc.dpiAwarenes);
    m_initialized = true;
    return true;
}

void WindowService::Shutdown()
{
    core::LockGuard<core::Mutex> lock(m_mutex);
    for (auto& itr : m_windows)
    {
        itr.second.reset();
    }

    m_windows.clear();

    m_initialized = true;
    if (m_threadActive == this)
    {
        m_threadActive = nullptr;
    }
}

bool WindowService::PeekMessages(WindowID windowID)
{
    m_threadActive = this;
    auto itr = m_windows.find(windowID);
    if (itr == m_windows.end())
    {
        return false;
    }

    Window* window = static_cast<Window*>(itr->second.get());
    HWND hwnd = window->GetHwnd();
    bool result = false;

    MSG msg {};
    while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
    {
        result = true;
        if (msg.message == WM_QUIT)
        {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return result;
}

WindowPtr WindowService::RegisterWindow(const IWindow::Desc& desc)
{
    WindowPtr window = Window::Create(desc);
    Window* nativeWindow = static_cast<Window*>(window.get());
    nativeWindow->SetEventFilter([this](WindowID windowID, WindowEventType type)
    {
        return this->HasCallback(windowID, type);
    });
    window->SetEventCallback([this](const WindowEventBase* event)
    {
        this->SendEventListners(event);
    });

    core::LockGuard<Mutex> lock(m_mutex);
    WindowID id = window->GetWindowID();
    auto itr = m_windows.find(id);
    if (itr == m_windows.end())
    {
        m_windows[id] = std::move(window);
    }
    return m_windows[id];
}

bool WindowService::TryEnumerateMonitors(core::TVector<MonitorInfo>& monitors) const
{
    return MonitorUtility::TryEnumerateMonitorInfo(monitors);
}

bool WindowService::TryEnumerateMonitorDetails(core::TVector<MonitorDetailInfo>& monitors) const
{
    return MonitorUtility::TryEnumerateDetailMonitorInfo(monitors);
}


std::optional<MonitorInfo> WindowService::GetMonitorFromWindow(UUID id) const
{
    auto itr = m_windows.find(id);
    if (itr != m_windows.end())
    {
        return itr->second->GetCurrentMonitorInfo();
    }
    return std::nullopt;
}

std::optional<MonitorInfo> WindowService::GetPrimaryMonitor() const
{
    MonitorInfo info = {};
    if (MonitorUtility::TryGetPrimaryMonitorInfo(info))
    {
        return info;
    }
    return std::nullopt;
}

void WindowService::SetAppUserAttention(bool flag)
{
    LockGuard<std::mutex> lock(m_mutex);
    for (auto& window : m_windows)
    {
        FLASHWINFO f = {};
        f.cbSize = sizeof(FLASHWINFO);
        NativeWindowHandlePtr handle = window.second->GetNativeHandle();
        f.hwnd = (HWND)handle->GetHandle();
        f.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
        f.uCount = 3;
        f.dwTimeout = 0;
        FlashWindowEx(&f);
    }
}

void WindowService::SetClipboardText(StringView text)
{
    if (!OpenClipboard(nullptr))
    {
        return;
    }
    EmptyClipboard();
    const size_t bytes = (text.size() + 1);
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hg)
    {
        void* ptr = GlobalLock(hg);
        if (ptr != nullptr)
        {
            memcpy(ptr, text.data(), bytes);
        }
        GlobalUnlock(hg);
        SetClipboardData(CF_TEXT, hg);
    }
    CloseClipboard();
}

core::String WindowService::GetClipboardText() const
{
    if (!OpenClipboard(nullptr))
    {
        return {};
    }
    HANDLE h = GetClipboardData(CF_TEXT);
    if (!h)
    {
        CloseClipboard(); return {};
    }
    char* p = (char*)GlobalLock(h);
    core::String out = p ? p : "";
    GlobalUnlock(h);
    CloseClipboard();
    return out;
}

void WindowService::SetDpiAwareness(DpiAwareness dpi)
{
   
    using SetProcessDpiAwarenessContextFn = BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT);
    static auto pSetDpiAwarenessCtx = (SetProcessDpiAwarenessContextFn)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetProcessDpiAwarenessContext");
    if (pSetDpiAwarenessCtx)
    {
        switch (dpi)
        {
        case DpiAwareness::Unaware:
            pSetDpiAwarenessCtx(DPI_AWARENESS_CONTEXT_UNAWARE);
            break;
        case DpiAwareness::SystemAware:
            pSetDpiAwarenessCtx(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
            break;
        case DpiAwareness::PerMonitorAware:
            pSetDpiAwarenessCtx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            break;
        }
        m_dpiAwareness = dpi;
    }
}

DpiAwareness WindowService::GetDpiAwareness() const
{
    return m_dpiAwareness;
}

void WindowService::SendEventListners(const WindowEventBase* event) const
{
    m_listner.SendEvent(event);
}

}
#endif
