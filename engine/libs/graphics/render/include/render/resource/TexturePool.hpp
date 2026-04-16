#pragma once

#include "rhi/Rhi.hpp"
#include "core/pattern/ResourcePool.hpp"

namespace slug::render
{

class TexturePool : public core::ResourcePool<uint64_t, rhi::TexturePtr>
{
};
using TexturePoolPtr = core::TReferencePtr<TexturePool>;

}
