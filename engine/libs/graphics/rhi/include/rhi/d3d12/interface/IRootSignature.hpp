#pragma once

#include "rhi/ResourceType.hpp"

namespace slug::rhi::d3d12
{
class IRootSignature : public IResource 
{
};
using RootSignaturePtr = core::TReferencePtr<IRootSignature>;

}