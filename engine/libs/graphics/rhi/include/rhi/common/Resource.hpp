#pragma once

#include "core/memory/ReferencePtr.hpp"

namespace slug 
{
namespace rhi
{
namespace object_types 
{
constexpr core::ObjectType SharedHandle = 0x00000001;

constexpr core::ObjectType rhi_D3D12_Device = 0x00020101;
constexpr core::ObjectType rhi_D3D12_CommandList = 0x00020102;

constexpr core::ObjectType D3D11_Device = 0x00010001;
constexpr core::ObjectType D3D11_DeviceContext = 0x00010002;
constexpr core::ObjectType D3D11_Resource = 0x00010003;
constexpr core::ObjectType D3D11_Buffer = 0x00010004;
constexpr core::ObjectType D3D11_RenderTargetView = 0x00010005;
constexpr core::ObjectType D3D11_DepthStencilView = 0x00010006;
constexpr core::ObjectType D3D11_ShaderResourceView = 0x00010007;
constexpr core::ObjectType D3D11_UnorderedAccessView = 0x00010008;

constexpr core::ObjectType D3D12_Device = 0x00020001;
constexpr core::ObjectType D3D12_CommandQueue = 0x00020002;
constexpr core::ObjectType D3D12_GraphicsCommandList = 0x00020003;
constexpr core::ObjectType D3D12_Resource = 0x00020004;
constexpr core::ObjectType D3D12_RenderTargetViewDescriptor = 0x00020005;
constexpr core::ObjectType D3D12_DepthStencilViewDescriptor = 0x00020006;
constexpr core::ObjectType D3D12_ShaderResourceViewGpuDescripror = 0x00020007;
constexpr core::ObjectType D3D12_UnorderedAccessViewGpuDescripror = 0x00020008;
constexpr core::ObjectType D3D12_RootSignature = 0x00020009;
constexpr core::ObjectType D3D12_PipelineState = 0x0002000a;
constexpr core::ObjectType D3D12_CommandAllocator = 0x0002000b;

constexpr core::ObjectType VK_Device = 0x00030001;
constexpr core::ObjectType VK_PhysicalDevice = 0x00030002;
constexpr core::ObjectType VK_Instance = 0x00030003;
constexpr core::ObjectType VK_Queue = 0x00030004;
constexpr core::ObjectType VK_CommandBuffer = 0x00030005;
constexpr core::ObjectType VK_DeviceMemory = 0x00030006;
constexpr core::ObjectType VK_Buffer = 0x00030007;
constexpr core::ObjectType VK_Image = 0x00030008;
constexpr core::ObjectType VK_ImageView = 0x00030009;
constexpr core::ObjectType VK_AccelerationStructureKHR = 0x0003000a;
constexpr core::ObjectType VK_Sampler = 0x0003000b;
constexpr core::ObjectType VK_ShaderModule = 0x0003000c;
constexpr core::ObjectType VK_RenderPass = 0x0003000d;
constexpr core::ObjectType VK_Framebuffer = 0x0003000e;
constexpr core::ObjectType VK_DescriptorPool = 0x0003000f;
constexpr core::ObjectType VK_DescriptorSetLayout = 0x00030010;
constexpr core::ObjectType VK_DescriptorSet = 0x00030011;
constexpr core::ObjectType VK_PipelineLayout = 0x00030012;
constexpr core::ObjectType VK_Pipeline = 0x00030013;
constexpr core::ObjectType VK_Micromap = 0x00030014;
}
}
}
