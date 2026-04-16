#pragma once

#include "engine_framework/ApplicationBase.hpp"

namespace slug::app
{

class Application : public engine_framework::ApplicationBase
{
public:
    struct SetupParam : engine_framework::ApplicationBase::SetupParam
    {
    };

    void Initialize(const SetupParam& param);
    void RunLoop();

private:
    

};
using ApplicationPtr = core::TReferencePtr<Application>;
}
