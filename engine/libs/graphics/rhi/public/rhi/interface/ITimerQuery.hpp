#pragma once

#include "rhi/ResourceType.hpp"
namespace slug::rhi
{
class ITimerQuery : public IResource
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(ITimerQuery, IResource)
};

using TimerQueryPtr = core::TReferencePtr<ITimerQuery>;
}