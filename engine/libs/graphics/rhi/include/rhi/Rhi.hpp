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
#include "core/Define.hpp"

#define SLUG_MEMORY_LABEL_SCOPE_GRAPHICS SLUG_MEMORY_LABEL_SCOPE(Graphics)
#define SLUG_MEMORY_LABEL_SCOPE_GRAPHICS_BUFFER SLUG_MEMORY_LABEL_SCOPE_GRAPHICS SLUG_MEMORY_SUBLABEL_SCOPE(Buffer)
#define SLUG_MEMORY_LABEL_SCOPE_GRAPHICS_TEXTURE SLUG_MEMORY_LABEL_SCOPE_GRAPHICS SLUG_MEMORY_SUBLABEL_SCOPE(Texture)
#define SLUG_MEMORY_LABEL_SCOPE_GRAPHICS_SAMPLER SLUG_MEMORY_LABEL_SCOPE_GRAPHICS SLUG_MEMORY_SUBLABEL_SCOPE(Sampler)
#define SLUG_MEMORY_LABEL_SCOPE_GRAPHICS_SHADER SLUG_MEMORY_LABEL_SCOPE_GRAPHICS SLUG_MEMORY_SUBLABEL_SCOPE(Shader)

namespace slug::rhi
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
