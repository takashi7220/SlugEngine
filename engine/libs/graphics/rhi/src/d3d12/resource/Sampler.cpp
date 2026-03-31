#include "rhi/d3d12/D3D12Utility.hpp"
#include "rhi/d3d12/resource/Context.hpp"
#include "rhi/d3d12/resource/Sampler.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
Sampler::Sampler(const Context& context, const SamplerDesc& desc)
    : m_context(context)
    , m_desc(desc)
    , m_d3d12desc {}
{
    uint32_t reductionType = D3D12Utility::ConvertSamplerReductionType(desc.reductionType);

    if (m_desc.maxAnisotropy > 1.0f)
    {
        m_d3d12desc.Filter = D3D12_ENCODE_ANISOTROPIC_FILTER(reductionType);
    }
    else
    {
        m_d3d12desc.Filter = D3D12_ENCODE_BASIC_FILTER(
            m_desc.minFilter ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT,
            m_desc.magFilter ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT,
            m_desc.mipFilter ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT,
            reductionType);
    }

    m_d3d12desc.AddressU = D3D12Utility::ConvertSamplerAddressMode(m_desc.addressU);
    m_d3d12desc.AddressV = D3D12Utility::ConvertSamplerAddressMode(m_desc.addressV);
    m_d3d12desc.AddressW = D3D12Utility::ConvertSamplerAddressMode(m_desc.addressW);

    m_d3d12desc.MipLODBias = m_desc.mipBias;
    m_d3d12desc.MaxAnisotropy = math::TMax((uint32_t)m_desc.maxAnisotropy, 1U);
    m_d3d12desc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
    m_d3d12desc.BorderColor[0] = m_desc.borderColor.r;
    m_d3d12desc.BorderColor[1] = m_desc.borderColor.g;
    m_d3d12desc.BorderColor[2] = m_desc.borderColor.b;
    m_d3d12desc.BorderColor[3] = m_desc.borderColor.a;
    m_d3d12desc.MinLOD = 0;
    m_d3d12desc.MaxLOD = D3D12_FLOAT32_MAX;
}

void Sampler::CreateDescriptor(size_t descriptor) const
{
    m_context.device->CreateSampler(&m_d3d12desc, { descriptor });
}

const SamplerDesc& Sampler::GetDesc() const
{
    return m_desc;
}

}
}
}
