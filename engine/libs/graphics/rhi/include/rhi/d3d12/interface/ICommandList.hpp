#pragma once

#include <d3d12.h>
#include "rhi/ResourceType.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/ICommandList.hpp"

namespace slug 
{
namespace rhi 
{
namespace d3d12 
{
class ICommandList : public rhi::ICommandList {
public:
    virtual bool AllocateUploadBuffer(size_t size, void** pCpuAddress, D3D12_GPU_VIRTUAL_ADDRESS* pGpuAdress) = 0;
    virtual bool CommitDescriptorHeaps() = 0;
    virtual D3D12_GPU_VIRTUAL_ADDRESS GetBufferGpuVA(IBuffer* buffer) = 0;

    virtual void UpdateGraphicsVolatileBuffers() = 0;
    virtual void UpdateComputeVolatileBuffers() = 0;
};
using CommandListPtr = core::TReferencePtr<ICommandList>;

}
}
}