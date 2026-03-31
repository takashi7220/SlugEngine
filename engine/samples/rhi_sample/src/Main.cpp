#include "render/GraphicsContext.hpp"
#include "render/Window.hpp"

int main()
{
    slug::render::WindowParam windowParameter;
    windowParameter.width = 1280;
    windowParameter.height = 720;
    windowParameter.name = "rhi_sample";

    slug::render::GraphicsParameter parameter;
    parameter.enableDebug = true;
    parameter.graphicsApiType = slug::render::GraphicsApiType::D3D12;
    parameter.width = 1280;
    parameter.height = 720;
    parameter.window = slug::render::WindowUtility::CreateWindowsWindow(windowParameter);

    SLUG_MAYBE_UNUSED auto context = slug::render::GraphicsContext::Create(parameter);

    return 0;
}
