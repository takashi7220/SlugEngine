#include "core/debug/CompileMessage.hpp"
#include "core/window/WindowUtility.hpp"
#include "core/debug/Logger.hpp"


namespace slug::app
{

class RenderTestLogger : public core::ILoggerController
{
public:
    void Write(core::LogType type, core::ILogCategory* category, core::StringView message) override
    {
    }
};
RenderTestLogger logger;

Application* Application::Create(const Param& param)
{
    Application* app = new Application();
    app->Initialize(param);
    return app;
}

void Application::Initialize(const Param& param)
{
    m_drawContext = CreateDrawContext(param);
    m_windowManager = CreateWindowManager(param);
    m_mainWindow = GenerateWindow(param, m_drawContext, m_windowManager);
    AddWindowEventListner();

    core::TaskSystem::Config config;
    config.workerCount = param.threadCount;
    m_taskSystem = core::TaskSystem::Create(config);

    core::Logger::Create();
    core::Logger::BindController(&logger);
}

void Application::RunLoop()
{
    m_mainWindow->Show(true);
    while (!m_shouldExit)
    {
        m_windowManager->PumpOSMessages();



        if (m_mainWindow->GetAllowPresent())
        {
            m_drawContext->Present(m_mainWindow->GetName(), 0);
        }
    }
}

render::DrawContextPtr Application::CreateDrawContext(const Param& param) const
{
    render::DrawContext::SetupParam desc;
    desc.enableDebug = param.enableDebug;
    desc.apiType = render::GraphicsApiType::D3D12;
    return render::DrawContext::Create(desc);
}

core::WindowManagerPtr Application::CreateWindowManager(const Param& param) const
{
    core::WindowManagerDesc desc;
    desc.dpiAwarenes = core::DpiAwareness::PerMonitorAware;
    desc.type = core::WindowPlatformType::Windows;
    return core::WindowUtility::CreateWindowManager(desc);
}

core::WindowPtr Application::GenerateWindow(const Param& param, const render::DrawContextPtr& context, core::WindowManagerPtr& windowManager) const
{
    core::WindowDesc desc;
    desc.title = param.name;
    desc.width = param.width;
    desc.height = param.height;
    core::WindowPtr window = windowManager->AddWindow(desc);

    render::DrawContext::WindowParam windowParam;
    windowParam.handle = window->GetNativeHandle()->GetHandle();
    windowParam.width = param.width;
    windowParam.height = param.height;
    windowParam.name = param.name.c_str();
    context->AddWindow(windowParam);

    return window;
}

void Application::AddWindowEventListner()
{
    m_windowManager->AddEventListener([this](const core::WindowEvent& e)
    {
        // CloseEvent
        if (std::holds_alternative<core::WindowCloseEvent>(e))
        {
            const core::WindowCloseEvent& closeEvent = std::get<core::WindowCloseEvent>(e);
            if (m_mainWindow->GetUUID() == closeEvent.id)
            {
                m_shouldExit = true;
            }
        }
    });
}
}
