#pragma once

#include "rhi/Rhi.hpp"
#include "core/pattern/ResourcePool.hpp"

namespace slug
{
namespace render
{

class BufferPool : public core::ResourcePool<uint64_t, rhi::BufferPtr>
{
};
using BufferPoolPtr = core::TReferencePtr<BufferPool>;
}
}
