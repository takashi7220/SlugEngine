#pragma once

#include "core/window/WindowUtility.hpp"
#include "core/window/null/WindowManager.hpp"

#if defined(PLATFORM_WINDOWS)
#include "core/window/win/WindowManager.hpp"
#endif

namespace slug::core
{

void WindowUtility::RegisterWindowManager(const WindowManagerDesc& desc, ServiceProvider& serviceProvider)
{
    switch (desc.type)
    {
#if defined(PLATFORM_WINDOWS)
    case WindowPlatformType::Windows:
        serviceProvider.AddSingleton<IWindowManager>([desc](ServiceProvider& provider)
        {
            IServicePtr service(new win::WindowManager(desc));
            return service;
        });
        break;
#endif
    case WindowPlatformType::Null:
        [[fallthrough]];
    default:
        serviceProvider.AddSingleton<IWindowManager>([desc](ServiceProvider& provider)
        {
            IServicePtr service(new null::WindowManager(desc));
            return service;
        });
        break;
    }
}

}
