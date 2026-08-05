#pragma once

#include "core/debug/Logger.hpp"
#include <d3d12.h>
namespace slug::rhi::d3d12
{

#define SLUG_D3D12_HRESULT_ASSERT(hr, device)                                                              \
    core::String message;                                                                                  \
    if (device != nullptr) {                                                                               \
        HRESULT reason = device->GetDeviceRemovedReason();                                                 \
        message = core::StringUtility::Snprintf("HRESULT failed, hr=0x%08X, reason=0x%08X", hr, reason);   \
    }                                                                                                      \
    else {                                                                                                 \
        message = core::StringUtility::Snprintf("HRESULT failed, hr=0x%08X", hr);                          \
    }                                                                                                      \
    SLUG_ASSERT_MSG(false, message.c_str());                                                               \


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
