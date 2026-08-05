#pragma once

#include "core/window/WindowUtility.hpp"
#include "core/window/null/WindowService.hpp"

#if defined(PLATFORM_WINDOWS)
#include "core/window/win/WindowService.hpp"
#endif

namespace slug::core
{

void WindowUtility::RegisterWindowService(const IWindowService::Desc& desc, ServiceProvider& serviceProvider)
{
    switch (desc.type)
    {
#if defined(PLATFORM_WINDOWS)
    case WindowPlatformType::Windows:
        serviceProvider.AddSingleton<IWindowService>([desc](ServiceProvider& provider)
        {
            IServicePtr service(NewObject<win::WindowService>(desc));
            return service;
        });
        break;
#endif
    case WindowPlatformType::Null:
        [[fallthrough]];
    default:
        serviceProvider.AddSingleton<IWindowService>([desc](ServiceProvider& provider)
        {
            IServicePtr service(NewObject<null::WindowService>(desc));
            return service;
        });
        break;
    }
}

}
