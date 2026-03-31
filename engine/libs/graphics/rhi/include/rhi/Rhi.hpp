#pragma once

#include "rhi/interface/IDevice.hpp"
#include "rhi/interface/IHeap.hpp"
#include "rhi/interface/ITexture.hpp"
#include "rhi/interface/IInputLayout.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/IShader.hpp"
#include "rhi/interface/ISampler.hpp"
#include "rhi/interface/IFrameBuffer.hpp"
#include "rhi/interface/IContext.hpp"
#include "core/debug/Logger.hpp"

namespace slug 
{
namespace rhi 
{

class RHILogCategory : public slug::core::LogCategoryBase
{
public:
    virtual core::StringView GetName() const override
    {
        return "Rhi";
    }
};
}
}
