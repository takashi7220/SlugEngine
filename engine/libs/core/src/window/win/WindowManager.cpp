#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include <shellapi.h>

#include "core/debug/CompileMessage.hpp"
#include "core/window/win/Window.hpp"
#include "core/window/win/WindowManager.hpp"
#include "core/window/win/MonitorUtility.hpp"

namespace slug::core::win
{

thread_local WindowManager* WindowManager::m_threadActive = nullptr;

WindowManager::WindowManager(const WindowManagerDesc& desc)
    : IWindowManager()
{
    this->Initialize(desc);
}

bool WindowManager::Initialize(const WindowManagerDesc& desc)
{
    m_threadActive = this;
    SetDpiAwareness(desc.dpiAwarenes);
    m_initialized = true;
    return true;
}

void WindowManager::Shutdown()
{
    core::LockGuard<core::Mutex> lock(m_mutex);
    for (auto& itr : m_windows)
    {
        itr.second.reset();
    }

    m_windows.clear();
    m_listeners.clear();

    m_initialized = true;
    if (m_threadActive == this)
    {
        m_threadActive = nullptr;
    }
}

void WindowManager::PumpOSMessages()
{
    m_threadActive = this;

    MSG msg {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            SLUG_COMPILE_NOTE_MESSAGE("Calls Close on all windows if necessary.");
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

WindowPtr WindowManager::AddWindow(const WindowDesc& desc)
{
    WindowPtr window = Window::Create(desc);
    window->SetEventCallback([this](const WindowEvent& event)
    {
        this->SendEventListners(event);
    });

    core::LockGuard<Mutex> lock(m_mutex);
    UUID id = window->GetUUID();
    auto itr = m_windows.find(id);
    if (itr == m_windows.end())
    {
        m_windows[id] = std::move(window);
    }
    return m_windows[id];
}

bool WindowManager::TryEnumerateMonitors(core::TVector<MonitorInfo>& monitors) const
{
    return MonitorUtility::TryEnumerateMonitorInfo(monitors);
}

bool WindowManager::TryEnumerateMonitorDetails(core::TVector<MonitorDetailInfo>& monitors) const
{
    return MonitorUtility::TryEnumerateDetailMonitorInfo(monitors);
}


std::optional<MonitorInfo> WindowManager::GetMonitorFromWindow(UUID id) const
{
    auto itr = m_windows.find(id);
    if (itr != m_windows.end())
    {
        return itr->second->GetCurrentMonitorInfo();
    }
    return std::nullopt;
}

std::optional<MonitorInfo> WindowManager::GetPrimaryMonitor() const
{
    MonitorInfo info = {};
    if (MonitorUtility::TryGetPrimaryMonitorInfo(info))
    {
        return info;
    }
    return std::nullopt;
}

void WindowManager::SetAppUserAttention(bool flag)
{
    LockGuard<std::mutex> lock(m_mutex);
    for (auto& window : m_windows)
    {
        FLASHWINFO f { sizeof(FLASHWINFO) };
        NativeWindowHandlePtr handle = window.second->GetNativeHandle();
        f.hwnd = (HWND)handle->GetHandle();
        f.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
        f.uCount = 3;
        f.dwTimeout = 0;
        FlashWindowEx(&f);
    }
}

void WindowManager::SetClipboardText(StringView text)
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

core::String WindowManager::GetClipboardText() const
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

void WindowManager::SetDpiAwareness(DpiAwareness dpi)
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

DpiAwareness WindowManager::GetDpiAwareness() const
{
    return m_dpiAwareness;
}

void WindowManager::SendEventListners(const WindowEvent& event) const
{
    for (auto& itr : m_listeners)
    {
        itr.second(event);
    }
}

}
#endif
