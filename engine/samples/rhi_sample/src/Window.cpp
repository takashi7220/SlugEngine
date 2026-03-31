#pragma once

#include <Windows.h>
#include <windowsx.h>
#include "render/Window.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/container/Vector.hpp"
namespace slug
{
namespace render
{

class WindowsWindow final: public IWindow
{
public:
    WindowsWindow() = default;
    ~WindowsWindow() = default;
    bool Create(WindowParam param) override;
    void* GetWindowHandle() override;
    void SetWindowPos(int32_t x, int32_t y) override;
    void GetWindowSize(bool adjustSize, int32_t& x, int32_t& y) override;
    bool CheckWindowShouldClose() override;
    void Shutdown() override;
    void RegistWindowCallback(WindowCallback callback) override;
    void Callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    void CleanCallback();

private:
    HWND m_windowHandle = {};
    MSG m_msg = {};
    WindowParam m_param = {};
    core::TVector<WindowCallback> m_callbacks = {};
};

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (ptr)
    {
        WindowsWindow* window = reinterpret_cast<WindowsWindow*>(ptr);
        window->Callback(hwnd, msg, wparam, lparam);
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WindowsWindow::Create(WindowParam param)
{
    m_param = param;
    WNDCLASSEX w = {};
    w.cbSize = sizeof(WNDCLASSEX);
    w.lpfnWndProc = (WNDPROC)WindowProcedure;
    w.lpszClassName = param.name.c_str();
    w.hInstance = ::GetModuleHandle(0);
    ::RegisterClassEx(&w);

    RECT wrc = { 0,0, (LONG)param.width, (LONG)param.height };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
    m_windowHandle = ::CreateWindow(w.lpszClassName,
        param.name.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr,
        nullptr,
        w.hInstance,
        this);

    ShowWindow(m_windowHandle, SW_SHOWNORMAL);
    SetWindowLongPtr(m_windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    DragAcceptFiles(m_windowHandle, TRUE);
    ShowCursor(FALSE);
    return true;
}

void* WindowsWindow::GetWindowHandle()
{
    return m_windowHandle;
}

void WindowsWindow::SetWindowPos(int32_t x, int32_t y)
{
    ::MoveWindow(m_windowHandle, x, y, m_param.width, m_param.height, TRUE);
}

void WindowsWindow::GetWindowSize(bool adjustSize, int32_t& x, int32_t& y)
{
    RECT rect = {};
    if (adjustSize && ::GetClientRect(m_windowHandle, &rect))
    {
        x = rect.right - rect.left;
        y = rect.bottom - rect.top;
    }
    else if (::GetWindowRect(m_windowHandle, &rect))
    {
        x = rect.right - rect.left;
        y = rect.bottom - rect.top;
    }
}

bool WindowsWindow::CheckWindowShouldClose()
{
    if (::PeekMessage(&m_msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&m_msg);
        ::DispatchMessage(&m_msg);
    }

    CleanCallback();
    return (m_msg.message == WM_QUIT);
}


void WindowsWindow::Shutdown()
{
    ::DestroyWindow(m_windowHandle);
}

void WindowsWindow::RegistWindowCallback(WindowCallback callback)
{
    m_callbacks.push_back(callback);
}

void WindowsWindow::Callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    WindowMessage message = {};
    message.messageID = msg;
    message.lParam = lparam;
    message.wParam = static_cast<uint32_t>(wparam);

    for (size_t i = 0; i < m_callbacks.size(); i++)
    {
        if (m_callbacks[i])
        {
            m_callbacks[i](message);
        };
    }
}

void WindowsWindow::CleanCallback()
{
    auto itr = m_callbacks.begin();
    while (itr != m_callbacks.end())
    {
        if (*itr == nullptr)
        {
            itr = m_callbacks.erase(itr);
        }
        itr++;
    }
}

core::TReferencePtr<IWindow> WindowUtility::CreateWindowsWindow(WindowParam param)
{
    core::TReferencePtr<IWindow> window = core::MakeReference<WindowsWindow>();
    window->Create(param);
    return window;
}
}
}
