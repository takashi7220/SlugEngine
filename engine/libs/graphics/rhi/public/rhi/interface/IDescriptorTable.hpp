#pragma once

#include "rhi/ResourceType.hpp"
#include "rhi/interface/IBindingSet.hpp"

namespace slug::rhi
{
class IDescriptorTable : public IBindingSet
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IDescriptorTable, IBindingSet)
    virtual uint32_t GetCapacity() const = 0;
};
using DescriptorTablePtr = core::TReferencePtr<IDescriptorTable>;

}