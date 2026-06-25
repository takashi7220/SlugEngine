#include "render_test/Application.hpp"

int main(int argc, const char* argv)
{
    slug::app::Application::Param param;
    param.name = "render_test";
    param.width = 1600;
    param.height = 900;
    param.threadCount = std::thread::hardware_concurrency();

    slug::app::ApplicationPtr app = slug::app::Application::Create(param);
    app->RunLoop();
}
