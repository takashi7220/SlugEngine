#pragma once

#include "rhi/ResourceType.hpp"
namespace slug
{
namespace rhi
{
class IEventQuery : public IResource 
{
};

using EventQueryPtr = core::TReferencePtr<IEventQuery>;
}
}