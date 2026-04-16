#pragma once

#include "rhi/ResourceType.hpp"

namespace slug::rhi
{

struct SamplerDesc
{
    Color borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float maxAnisotropy = 1.f;
    float mipBias = 0.f;
    bool minFilter = true;
    bool magFilter = true;
    bool mipFilter = true;
    SamplerAddressMode addressU = SamplerAddressMode::Clamp;
    SamplerAddressMode addressV = SamplerAddressMode::Clamp;
    SamplerAddressMode addressW = SamplerAddressMode::Clamp;
    SamplerReductionType reductionType = SamplerReductionType::Standard;
};

class ISampler : public IResource
{
public:
    virtual const SamplerDesc& GetDesc() const = 0;
};

using SamplerPtr = core::TReferencePtr<ISampler>;

}