#pragma once

#include "render_core/service/RenderService.hpp"
#include "core/window/WindowUtility.hpp"

namespace slug::render_unit
{
class ApplicationTest final
{
public:
    ApplicationTest();
    ~ApplicationTest();
    void Initialize();
    void Update();
    void Terminate();
    void Run();

private:
    slug::core::ServiceProvider m_serviceProvider;
    slug::render_core::RenderServicePtr m_renderService = nullptr;
    slug::core::WindowServicePtr m_windowService = nullptr;
    slug::core::WindowPtr m_window = nullptr;
    bool m_shouldShutdown = false;
    bool m_initialized = false;
};

}
