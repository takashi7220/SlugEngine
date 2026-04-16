#pragma once

#include "core/debug/Logger.hpp"
#include <d3d12.h>
namespace slug::rhi::d3d12
{

using DescriptorIndex = uint32_t;
using RootParameterIndex = uint32_t;
constexpr DescriptorIndex INVALID_DESCRIPTOR_INDEX = ~0u;

class D3D12LogCategory : public core::LogCategoryBase 
{
public:
    core::StringView GetName() const override 
    {
        return "RHI_D3D12";
    }
};
}
