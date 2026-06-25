#pragma once

#include "rhi/ResourceType.hpp"
namespace slug::rhi
{
class IEventQuery : public IResource
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IEventQuery, IResource)
};

using EventQueryPtr = core::TReferencePtr<IEventQuery>;
}