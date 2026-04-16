#pragma once

#include "rhi/ResourceType.hpp"

namespace slug::rhi
{

struct BindingLayoutDesc
{
    ShaderType visibility = ShaderType::None;
    uint32_t registerSpace = 0;
    BindingLayoutItemArray bindings;
    VulkanBindingOffsets bindingOffsets;
};

struct BindlessLayoutDesc
{
    ShaderType visibility = ShaderType::None;
    uint32_t firstSlot = 0;
    uint32_t maxCapacity = 0;
    core::TStaticVector<BindingLayoutItem, 16> registerSpaces;
};

class IBindingLayout : public IResource 
{
public:
    virtual const BindingLayoutDesc* GetDesc() const = 0;
    virtual const BindlessLayoutDesc* GetBindlessDesc() const = 0;
};
using BindingLayoutPtr = core::TReferencePtr<IBindingLayout>;
using BindingLayoutVector = core::TStaticVector<BindingLayoutPtr, MAX_BINDING_LAYOUTS>;
}