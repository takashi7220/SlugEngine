#pragma once

#include "rhi/ResourceType.hpp"
namespace slug::rhi
{
class ITimerQuery : public IResource 
{
};

using TimerQueryPtr = core::TReferencePtr<ITimerQuery>;
}