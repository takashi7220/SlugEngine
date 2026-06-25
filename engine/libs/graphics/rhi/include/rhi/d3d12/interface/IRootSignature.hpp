#pragma once

#include "rhi/ResourceType.hpp"

namespace slug::rhi::d3d12
{
class IRootSignature : public IResource
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IRootSignature, IResource)
};
using RootSignaturePtr = core::TReferencePtr<IRootSignature>;

}