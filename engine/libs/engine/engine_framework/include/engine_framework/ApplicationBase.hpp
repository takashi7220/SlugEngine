#pragma once

#include "core/task/TaskSystem.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::engine_framework
{

class ApplicationBase : public core::ReferenceObject
{
public:
    struct SetupParam
    {
        core::FixedString<32> name;
        uint32_t width = 1600;
        uint32_t height = 900;
        uint32_t threadCount = std::thread::hardware_concurrency();
        bool enableDebug;
    };

    explicit ApplicationBase();
    virtual ~ApplicationBase();

    virtual bool Initialize(const SetupParam& setupParam);
    virtual void Update();
    virtual void Terminate();
private:
    core::ServiceProvider m_serviceProvider;
};
}
