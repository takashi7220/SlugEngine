#pragma once

#include "rhi/ResourceType.hpp"
namespace slug::rhi
{
class IEventQuery : public IResource 
{
};

using EventQueryPtr = core::TReferencePtr<IEventQuery>;
}