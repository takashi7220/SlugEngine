#pragma once

#include "rhi/interface/IDescriptorTable.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/ResourceType.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

class DescriptorTable : public IDescriptorTable
{
public:
    DescriptorTable(DeviceResources& resources);
    ~DescriptorTable() override;
    const BindingSetDesc* GetDesc() const override;
    IBindingLayout* GetLayout() const override;
    uint32_t GetCapacity() const override;

public:
    uint32_t capacity = 0;
    DescriptorIndex firstDescriptor = 0;

private:
    DeviceResources& m_resources;
};

}
}
}
