#include "engine_framework/ApplicationBase.hpp"
#include "core/task/TaskSystem.hpp"
#include "core/window/IWindowManager.hpp"

#if defined(PLATFORM_WINDOWS)
#include "core/window/win/WindowManager.hpp"
#endif

namespace slug::engine_framework
{
ApplicationBase::ApplicationBase()
{

}

ApplicationBase::~ApplicationBase()
{

}

bool ApplicationBase::Initialize(const SetupParam& setupParam)
{
    core::TaskSystemDesc taskSystemDesc = {};
    taskSystemDesc.workerCount = setupParam.threadCount;
    m_serviceProvider.AddSingleton<core::TaskSystem>(taskSystemDesc);

    core::WindowManagerDesc windowManagerDesc = {};
    windowManagerDesc.dpiAwarenes = core::DpiAwareness::PerMonitorAware;
    windowManagerDesc.type = core::WindowPlatformType::Windows;
    m_serviceProvider.AddSingleton<core::IWindowManager>(windowManagerDesc, core::win::WindowManager::Create);
}

void ApplicationBase::Update()
{

}

void ApplicationBase::Terminate()
{

}

}
