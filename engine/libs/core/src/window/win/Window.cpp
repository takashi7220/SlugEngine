#if defined(PLATFORM_WINDOWS)
#include "core/window/win/Window.hpp"
#include "core/window/win/MonitorUtility.hpp"
#include "core/container/Vector.hpp"
#include "core/debug/CompileMessage.hpp"
namespace slug::core::win
{

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    Window* self = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (self)
    {
        LRESULT outResult = 0;
        bool handled = false;
        self->HandleMessage(msg, wp, lp, outResult, handled);
        if (handled)
        {
            return outResult;
        }
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

Window* Window::Create(const WindowDesc& desc)
{
    Window* window = new Window();
    window->SetName(desc.title.c_str());
    window->m_desiredWidth = desc.width;
    window->m_desiredHeight = desc.height;
    window->m_decorated = desc.decorated;
    window->m_resizable = desc.resizable;
    window->CreateWindowInternal(desc);
    return window;
}

Window::Window()
{

}

Window::~Window()
{

}

StringView Window::GetTitle() const
{
    int32_t len = GetWindowTextLengthW(m_hwnd);
    core::WString ws(len, L'\0');
    GetWindowTextW(m_hwnd, ws.data(), len + 1);
    return StringUtility::GetString(ws.c_str());
}

void Window::SetTitle(StringView title)
{
    core::WString ws = StringUtility::GetWstring(title.data());
    SetWindowTextW(m_hwnd, ws.c_str());
}

core::WindowRect Window::ClientRect() const
{
    RECT rc{};
    GetClientRect(m_hwnd, &rc);
    return {0, 0, rc.right - rc.left, rc.bottom - rc.top};
}

core::WindowRect Window::WindowRect() const
{
    RECT rc {};
    GetWindowRect(m_hwnd, &rc);
    return { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };
}
math::Vector2 Window::ContentScale() const
{
    float s = QueryDpiScale();
    return { s, s };
}

bool Window::IsVisible() const
{
    return IsWindowVisible(m_hwnd) != 0;
}

bool Window::IsMinimized() const
{
    return IsIconic(m_hwnd) != 0;
}

bool Window::IsFocused() const
{
    return GetForegroundWindow() == m_hwnd;
}

WindowMode Window::GetWindowMode() const
{
    return m_mode;
}

void Window::SetWindowMode(WindowMode mode, core::StringView targetName)
{
    MonitorInfo info = {};
    if (!MonitorUtility::TryGetMonitorInfo(targetName, info))
    {
        info = GetCurrentMonitorInfo();
    }

    SetWindowModeInternal(mode, info);

}

void Window::Show(bool s)
{
    ShowWindow(m_hwnd, s ? SW_SHOW : SW_HIDE);
    m_shown = s;
}

void Window::Minimize(bool m)
{
    ShowWindow(m_hwnd, m ? SW_MINIMIZE : SW_RESTORE);
}

void Window::Focus()
{
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}

void Window::Resize(int32_t width, int32_t height)
{
    RECT rc { 0,0,width,height };
    AdjustWindowRectEx(&rc, m_style, FALSE, m_exStyle);
    SetWindowPos(m_hwnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void Window::Move(int32_t x, int32_t y)
{
    SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void Window::SetCursorMode(CursorMode mode)
{
    m_cursorMode = mode;
    UpdateCursorClip();
}

void Window::SetCursorVisible(bool flag)
{
    m_cursorVisible = flag;
    CURSORINFO ci { sizeof(CURSORINFO) };
    GetCursorInfo(&ci);
    bool shown = (ci.flags & CURSOR_SHOWING) != 0;
    if (flag != shown)
    {
        int tgt = flag ? 1 : -1;
        for (int i = 0; i < 8; i++)
        {
            ShowCursor(tgt > 0);
        }

        for (int i = 0; i < 8; i++)
        {
            ShowCursor(tgt < 0 ? FALSE : TRUE);
        }
    }
}

void Window::SetCaptionDragEnable(bool flag)
{
    m_captionDragEnabled = flag;
}

void Window::SetEventCallback(EventCallback event)
{
    m_eventCallback = event;
}

void Window::SetHitTestCallback(HitTestFunc func)
{
    m_hitTestCallback = func;
}

void Window::SetFileDropEnabled(bool flag)
{
    m_fileDropEnabled = flag;
    DragAcceptFiles(m_hwnd, flag ? TRUE : FALSE);
}

NativeWindowHandlePtr Window::GetNativeHandle() const
{
    WinWindowHandle* winHandle = new WinWindowHandle();
    winHandle->m_windowHandle = m_hwnd;
    NativeWindowHandlePtr handle = winHandle;
    return handle;
}

void Window::SetSwapchainRecreateCallback(SwapchainRecreateCallback cb)
{
    m_swapchainRecreateCallback = cb;
}

bool Window::GetAllowPresent() const
{
    return m_allowPresent;
}

void Window::SetAllowPresent(bool flag)
{
    m_allowPresent = flag;
}


MonitorInfo Window::GetCurrentMonitorInfo() const
{
    HMONITOR monitor = GetCurrentMonitor();
    return MonitorUtility::GetMonitorInfomation(monitor);
}

MonitorDetailInfo Window::GetCurrentMonitorDetailInfo() const
{
    HMONITOR monitor = GetCurrentMonitor();
    return MonitorUtility::GetMonitorDetailInfomation(monitor);
}


void Window::RequestSwapchainRecreate()
{
    if (m_swapchainRecreateCallback != nullptr)
    {
        m_swapchainRecreateCallback(*this);
    }
}

void Window::HandleMessage(UINT msg, WPARAM wp, LPARAM lp, LRESULT& outResult, bool& handled)
{
    switch (msg)
    {
    case WM_CLOSE:
        {
            if (m_eventCallback)
            {
                WindowCloseEvent event { GetUUID() };
                m_eventCallback(event);
            }
            handled = false;
            return;
        }

    case WM_NCCREATE:
        {
            handled = false;
            return;
        }

    case WM_SHOWWINDOW:
        {
            m_shown = (BOOL)wp != FALSE;
            if (m_eventCallback)
            {
                WindowShowEvent event { GetUUID(), m_shown };
                m_eventCallback(event);
            }
            handled = false;
            return;
        }

    case WM_SETFOCUS:
        {
            if (m_eventCallback)
            {
                WindowFocusEvent event { GetUUID(), true };
                m_eventCallback(event);
            }
            handled = false;
            return;
        }

    case WM_KILLFOCUS:
        {
            if (m_eventCallback)
            {
                WindowFocusEvent event { GetUUID(), false };
                m_eventCallback(event);
            }
            handled = false;
            return;
        }

    case WM_MOVE:
        {
            RECT rc {};
            GetWindowRect(m_hwnd, &rc);
            if (m_eventCallback)
            {
                WindowMoveEvent event { GetUUID(), rc.left, rc.top };
                m_eventCallback(event);
            }
            handled = false;
            return;
        }
    case WM_SIZE:
        {
            if (wp == SIZE_MINIMIZED)
            {
                m_allowPresent = false;
                if (m_eventCallback)
                {
                    WindowMinimizeEvent event { GetUUID(), true };
                    m_eventCallback(event);
                }
            }
            else
            {
                m_allowPresent = true;
                RECT rc {}; GetClientRect(m_hwnd, &rc);
                float dpi = QueryDpiScale();
                if (m_eventCallback)
                {
                    WindowResizeEvent event { GetUUID(), rc.right - rc.left, rc.bottom - rc.top, dpi };
                    m_eventCallback(event);
                }
                RequestSwapchainRecreate();
            }
            handled = false;
            return;
        }

    case WM_DPICHANGED:
        {
            RECT* suggested = reinterpret_cast<RECT*>(lp);
            SetWindowPos(m_hwnd, nullptr, suggested->left, suggested->top, suggested->right - suggested->left, suggested->bottom - suggested->top, SWP_NOZORDER | SWP_NOACTIVATE);

            float dpi = QueryDpiScale();
            if (m_eventCallback)
            {
                WindowDpiChangedEvent event { GetUUID(), dpi };
                m_eventCallback(event);
            }

            RequestSwapchainRecreate();
            handled = true;
            outResult = 0;
            return;
        }

    case WM_ENTERSIZEMOVE:
        {
            handled = false;
            return;
        }

    case WM_EXITSIZEMOVE:
        {
            RECT rc {};
            GetClientRect(m_hwnd, &rc);
            RequestSwapchainRecreate();
            handled = false;
            return;
        }

    case WM_GETMINMAXINFO:
        {
            if (!m_resizable)
            {
                auto* mmi = reinterpret_cast<MINMAXINFO*>(lp);
                // 最小最大を同値にするとリサイズ不可に近い挙動
                RECT rc { 0,0,m_desiredWidth, m_desiredHeight };
                AdjustWindowRectEx(&rc, m_style, FALSE, m_exStyle);
                int w = rc.right - rc.left, h = rc.bottom - rc.top;
                mmi->ptMinTrackSize = { w, h };
                mmi->ptMaxTrackSize = { w, h };
            }
            handled = false;
            return;
        }

    case WM_NCHITTEST:
        {
            if (m_hitTestCallback)
            {
                POINTS pts = MAKEPOINTS(lp);
                POINT p { pts.x, pts.y };
                POINT c = p;
                ScreenToClient(m_hwnd, &c);
                WindowHitResult r = m_hitTestCallback({ (math::ValueType)c.x, (math::ValueType)c.y });

                switch (r)
                {
                case WindowHitResult::Caption:
                    outResult = HTCAPTION;
                    handled = true;
                    return;
                case WindowHitResult::Left:
                    outResult = HTLEFT;
                    handled = true;
                    return;
                case WindowHitResult::Right:
                    outResult = HTRIGHT;
                    handled = true;
                    return;
                case WindowHitResult::Top:
                    outResult = HTTOP;
                    handled = true;
                    return;
                case WindowHitResult::Bottom:
                    outResult = HTBOTTOM;
                    handled = true;
                    return;
                case WindowHitResult::TopLeft:
                    outResult = HTTOPLEFT;
                    handled = true;
                    return;
                case WindowHitResult::TopRight:
                    outResult = HTTOPRIGHT;
                    handled = true;
                    return;
                case WindowHitResult::BottomLeft:
                    outResult = HTBOTTOMLEFT;
                    handled = true;
                    return;
                case WindowHitResult::BottomRight:
                    outResult = HTBOTTOMRIGHT;
                    handled = true;
                    return;
                case WindowHitResult::Client:
                    outResult = HTCLIENT;
                    handled = true;
                    return;
                default: break;
                }
            }
            handled = false;
            return;
        }
    case WM_DROPFILES:
        {
            if (m_fileDropEnabled)
            {
                HDROP hdrop = (HDROP)wp;
                UINT count = DragQueryFileW(hdrop, 0xFFFFFFFF, nullptr, 0);
                core::TVector<core::FixedString<260>> paths;
                paths.reserve(count);
                for (UINT i = 0; i < count; ++i)
                {
                    wchar_t buf[MAX_PATH];
                    DragQueryFileW(hdrop, i, buf, MAX_PATH);
                    paths.push_back(StringUtility::GetString(buf).c_str());
                }

                if (m_eventCallback)
                {
                    FileDropEvent event { GetUUID(), paths };
                    m_eventCallback(event);
                }

                DragFinish(hdrop);
            }
            handled = true;
            outResult = 0;
        }
        return;

    case WM_SYSCOMMAND:
        {
            if (wp == SC_KEYMENU && m_captionDragEnabled)
            {
                handled = true;
                outResult = 0;
                return;
            }
            handled = false;
            return;
        }
    default:
        {
            handled = false;
            return;
        }
    }
}

void Window::CreateWindowInternal(const WindowDesc& desc)
{
    m_style = WS_OVERLAPPEDWINDOW;
    if (!m_decorated)
    {
        m_style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    }

    if (!m_resizable)
    {
        m_style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    }

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.lpszClassName = "SLUG_WINDOW";
    wc.hInstance = ::GetModuleHandle(0);
    ::RegisterClassEx(&wc);


    RECT rc(0, 0, desc.width, desc.height);
    AdjustWindowRectEx(&rc, m_style, FALSE, m_exStyle);
    int32_t w = rc.right - rc.left;
    int32_t h = rc.bottom - rc.top;

    core::WString title = core::StringUtility::GetWstring(desc.title.data());
    m_hwnd = CreateWindowExW(m_exStyle, L"SLUG_WINDOW", title.c_str(), m_style, CW_USEDEFAULT, CW_USEDEFAULT, w, h, nullptr, nullptr, wc.hInstance, nullptr);

    if (m_hwnd == nullptr)
    {
        SLUG_THROW_EXCEPTION("CreateWindowEx Failed");
    }

    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
    if (desc.transparent)
    {
        SLUG_COMPILE_TODO_MESSAGE("Not Implement Window Transparent")
    }
    GetWindowRect(m_hwnd, &m_windowedRect);
    SetWindowMode(desc.mode, desc.initialMonitor.c_str());

    if (desc.position.has_value())
    {
        Move((int32_t)desc.position->x, (int32_t)desc.position->y);
    }

    DragAcceptFiles(m_hwnd, FALSE);
    m_windowedMonitor = GetCurrentMonitor();
}
\
void Window::ApplyBorderless(const MonitorInfo& info)
{
    LONG_PTR style = GetWindowLongPtr(m_hwnd, GWL_STYLE);
    LONG_PTR exstyle = GetWindowLongPtr(m_hwnd, GWL_EXSTYLE);

    style &= ~(WS_OVERLAPPEDWINDOW);
    style |= WS_POPUP;

    SetWindowLongPtr(m_hwnd, GWL_STYLE, style);
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, exstyle);

    SetWindowPos(m_hwnd, HWND_TOP, info.x, info.y, info.width, info.height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

void Window::RestoreWindowedFromBorderless()
{
    SetWindowLongPtr(m_hwnd, GWL_STYLE, m_style);
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, m_exStyle);
    int32_t pos[2] = { (int32_t)m_windowedRect.left , (int32_t)m_windowedRect.top };
    int32_t size[2] = { (int32_t)(m_windowedRect.right - m_windowedRect.left), (int32_t)(m_windowedRect.bottom - m_windowedRect.top) };
    SetWindowPos(m_hwnd, nullptr, pos[0], pos[1], size[0], size[1], SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

float Window::QueryDpiScale() const
{
    using GetDpiForWindowFn = UINT(WINAPI*)(HWND);
    static GetDpiForWindowFn pGetDpiForWindow = (GetDpiForWindowFn)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    if (pGetDpiForWindow)
    {
        UINT dpi = pGetDpiForWindow(m_hwnd);
        return dpi / 96.0f;
    }

    HDC hdc = GetDC(m_hwnd);
    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(m_hwnd, hdc);
    return dpi / 96.0f;
}

HMONITOR Window::GetCurrentMonitor() const
{
    return MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
}

void Window::UpdateCursorClip()
{
    if (m_cursorMode == CursorMode::Disable || m_cursorMode == CursorMode::Confined)
    {
        RECT rc {}; GetClientRect(m_hwnd, &rc);
        POINT tl { rc.left, rc.top }, br { rc.right, rc.bottom };
        ClientToScreen(m_hwnd, &tl);
        ClientToScreen(m_hwnd, &br);
        RECT screenRect { tl.x, tl.y, br.x, br.y };
        ClipCursor(&screenRect);
        if (m_cursorMode == CursorMode::Disable)
        {
            SetCursorVisible(false);
        }
    }
    else
    {
        ClipCursor(nullptr);
        {
            if (m_cursorMode == CursorMode::Normal)
            {
                SetCursorVisible(true);
            }
        }
    }
}

bool Window::SetWindowModeInternal(WindowMode mode, const MonitorInfo& info)
{
    if (mode != m_mode)
    {
        return true;
    }

    if (mode == WindowMode::BorderlessFullscreen)
    {
        GetWindowRect(m_hwnd, &m_windowedRect);
        m_windowedMonitor = GetCurrentMonitor();
        ApplyBorderless(info);
        m_mode = mode;
        return true;
    }
    else if (mode == WindowMode::Windowed)
    {
        RestoreWindowedFromBorderless();
        m_mode = mode;
        return true;
    }
    else if (mode == WindowMode::ExclusiveFullscreen)
    {
        RestoreWindowedFromBorderless();
        m_mode = mode;
        return true;
    }

    return false;
}

}
#endif
