#pragma once

#include "rhi/Rhi.hpp"
#include "core/pattern/ResourcePool.hpp"

namespace slug
{
namespace render
{

class SamplerPool : public core::ResourcePool<uint64_t, rhi::SamplerPtr>
{
};
using SamplerPoolPtr = core::TReferencePtr<SamplerPool>;
}
}
