#pragma once
#include "rhi/interface/ISampler.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
class Sampler : public ISampler
{
public:
    Sampler(const Context& context, const SamplerDesc& desc);
    void CreateDescriptor(size_t descriptor) const;
    const SamplerDesc& GetDesc() const override;
private:
    const Context& m_context;
    const SamplerDesc m_desc;
    D3D12_SAMPLER_DESC m_d3d12desc;
};

}
}
}
