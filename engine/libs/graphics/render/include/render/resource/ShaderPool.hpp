#pragma once

#include "rhi/Rhi.hpp"
#include "core/pattern/ResourcePool.hpp"

namespace slug::render
{

class ShaderPool : public core::ResourcePool<uint64_t, rhi::ShaderPtr>
{
};
using ShaderPoolPtr = core::TReferencePtr<ShaderPool>;

}
