#pragma once
#include "rhi/d3d12/D3D12.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/Buffer.hpp"

namespace slug::rhi::d3d12
{
class Context
{
public:
    core::TReferencePtr<ID3D12Device> device;
    core::TReferencePtr<ID3D12Device2> device2;
    core::TReferencePtr<ID3D12Device5> device5;

    core::TReferencePtr<ID3D12CommandSignature> drawIndirectSignature;
    core::TReferencePtr<ID3D12CommandSignature> drawIndexedIndirectSignature;
    core::TReferencePtr<ID3D12CommandSignature> dispatchIndirectSignature;
    core::TReferencePtr<ID3D12QueryHeap> timerQueryHeap;
    core::TReferencePtr<Buffer> timerQueryResolveBuffer;
};

}
