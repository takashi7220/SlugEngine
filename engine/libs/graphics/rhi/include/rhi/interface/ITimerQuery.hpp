#pragma once

#include "rhi/ResourceType.hpp"
namespace slug
{
namespace rhi
{
class ITimerQuery : public IResource 
{
};

using TimerQueryPtr = core::TReferencePtr<ITimerQuery>;
}
}