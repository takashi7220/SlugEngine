#pragma once

#include "core/window/IWindowManager.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::core
{

class WindowUtility
{
public:
    static void RegisterWindowManager(const WindowManagerDesc& desc, ServiceProvider& serviceProvider);
};
}
