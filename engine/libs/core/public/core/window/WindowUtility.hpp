#pragma once

#include "core/window/IWindowService.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::core
{

class WindowUtility
{
public:
    static void RegisterWindowService(const IWindowService::Desc& desc, ServiceProvider& serviceProvider);
};
}
