
#include "rhi/d3d12/resource/DescriptorTable.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

DescriptorTable::DescriptorTable(DeviceResources& resources)
    :m_resources(resources)
{
}

DescriptorTable::~DescriptorTable()
{
    m_resources.shaderResourceViewHeap.ReleaseDescriptors(firstDescriptor, capacity);
}

const BindingSetDesc* DescriptorTable::GetDesc() const
{
    return nullptr;
}

IBindingLayout* DescriptorTable::GetLayout() const
{
    return nullptr;
}

uint32_t DescriptorTable::GetCapacity() const
{
    return capacity;
}

}
}
}
