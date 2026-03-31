#include "rhi/d3d12/resource/OpacityMicromap.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

OpacityMicromap::OpacityMicromap(const Context& context)
    : m_context(context)
{}

Object OpacityMicromap::GetNativeObject(ObjectType objectType)
{
    if (dataBuffer)
    {
        return dataBuffer->GetNativeObject(objectType);
    }

    return nullptr;
}

const rt::OpacityMicromapDesc& OpacityMicromap::GetDesc() const
{
    return desc;
}

bool OpacityMicromap::isCompacted() const
{
    return compacted;
}

uint64_t OpacityMicromap::GetDeviceAddress() const
{
    return dataBuffer->gpuVirtualAddress;
}

}
}
}
