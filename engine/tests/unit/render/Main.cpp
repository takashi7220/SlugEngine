#include "Application.hpp"
#include "core/window/WindowUtility.hpp"
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    int ret = 0;
    {
        try
        {
            slug::render_unit::ApplicationTest app;
            app.Initialize();
            app.Run();
        }
        catch (std::runtime_error e)
        {
            printf("%s\n", e.what());
            ret = 1;
        }
    }
    return ret;
}
