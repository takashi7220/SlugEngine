#pragma once

#include <d3d12.h>
#include "rhi/ResourceType.hpp"
#include "rhi/interface/IDevice.hpp"
#include "rhi/d3d12/interface/IDescriptorHeap.hpp"
#include "rhi/d3d12/interface/IRootSignature.hpp"

namespace slug::rhi::d3d12
{
enum class DescriptorHeapType 
{
    RenderTargetView,
    DepthStencilView,
    ShaderResrouceView,
    Sampler
};

class IDevice : public rhi::IDevice {
public:
    virtual RootSignaturePtr BuildRootSignature(const core::TStaticVector<BindingLayoutPtr, MAX_BINDING_LAYOUTS>& pipelineLayouts, bool allowInputLayout, bool local, const D3D12_ROOT_PARAMETER1* pCustomParameters = nullptr, uint32_t numCustomParameters = 0) = 0;
    virtual GraphicsPipelinePtr CreateHandleForNativeGraphicsPipeline(IRootSignature* root_signature, ID3D12PipelineState* pipeline_state, const GraphicsPipelineDesc& desc, const FrameBufferInfo& framebuffer_info) = 0;
    virtual MeshletPipelinePtr CreateHandleForNativeMeshletPipeline(IRootSignature* root_signature, ID3D12PipelineState* pipeline_state, const MeshletPipelineDesc& desc, const FrameBufferInfo& framebuffer_info) = 0;
    virtual IDescriptorHeap* GetDescriptorHeap(DescriptorHeapType heap_type) = 0;
};
using DevicePtr = core::TReferencePtr<IDevice>;

struct DeviceDesc 
{
    IMessageCallback* errorCb = nullptr;
    ID3D12Device* pDevice = nullptr;
    ID3D12CommandQueue* pGraphicsCommandQueue = nullptr;
    ID3D12CommandQueue* pComputeCommandQueue = nullptr;
    ID3D12CommandQueue* pCopyCommandQueue = nullptr;

    uint32_t renderTargetViewHeapSize = 1024;
    uint32_t depthStencilViewHeapSize = 1024;
    uint32_t shaderResourceViewHeapSize = 16384;
    uint32_t samplerHeapSize = 1024;
    uint32_t maxTimerQueries = 256;
};
}