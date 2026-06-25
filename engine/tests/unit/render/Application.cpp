#include "Application.hpp"

namespace slug::render_unit
{
ApplicationTest::ApplicationTest()
{
    slug::render_core::RenderService::Desc renderDesc = {};
    renderDesc.apiType = slug::render_core::GraphicsApiType::D3D12;
    renderDesc.enableDebug = false;
    m_serviceProvider.AddSingleton<slug::render_core::RenderService>(
        [renderDesc](slug::core::ServiceProvider& sp){
            return slug::core::NewObject<slug::render_core::RenderService>(renderDesc);
        });

    slug::core::IWindowService::Desc windowDesc = {};
    windowDesc.dpiAwarenes = slug::core::DpiAwareness::PerMonitorAware;
    windowDesc.type = slug::core::WindowPlatformType::Windows;
    slug::core::WindowUtility::RegisterWindowService(windowDesc, m_serviceProvider);
}

ApplicationTest::~ApplicationTest()
{
}

void ApplicationTest::Initialize()
{
    m_renderService = m_serviceProvider.GetRequiredService<slug::render_core::RenderService>();
    m_windowService = m_serviceProvider.GetRequiredService<slug::core::IWindowService>();

    {
        slug::core::IWindow::Desc desc = {};
        desc.title = "render_test";
        desc.width = 1280;
        desc.height = 720;
        m_window = m_windowService->RegisterWindow(desc);
    }

    {
        slug::core::WindowEventListner::RegisterParam destroyEvent = {};
        destroyEvent.targetWindow = m_window->GetWindowID();
        destroyEvent.type = slug::core::WindowEventType::WindowDestroy;
        destroyEvent.hookAllWindow = true;
        destroyEvent.callback = [this](const slug::core::WindowEventBase* event)
            {
                m_windowService->UnregisterWindow(event->id);               
                m_shouldShutdown = true;
            };
        m_windowService->RegisterCallback(destroyEvent);
    }

    {
        slug::render_core::RenderService::WindowParam param = {};
        param.name = m_window->GetName();
        param.width = m_window->GetWidth();
        param.height = m_window->GetHeight();
        param.handle = m_window->GetNativeHandle()->GetHandle();
        m_renderService->RegisterWindow(param);
    }
    m_initialized = true;
}

void ApplicationTest::Update()
{
    m_windowService->PeekMessages(m_window->GetWindowID());

    if (m_shouldShutdown || !m_window || !m_window->GetAllowPresent())
    {
        return;
    }

    m_renderService->Present(m_window->GetName(), 1);
}

void ApplicationTest::Terminate()
{
}

void ApplicationTest::Run()
{
    if (!m_initialized)
    {
        Initialize();
    }

    while (!m_shouldShutdown)
    {
        Update();
    }
    m_window.reset();
    Terminate();
}

}
