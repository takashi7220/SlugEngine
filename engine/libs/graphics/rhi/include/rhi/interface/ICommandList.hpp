#pragma once

#include "rhi/ResourceType.hpp"
#include "rhi/interface/IBindingLayout.hpp"
#include "rhi/interface/IBindingSet.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/IComputePipiline.hpp"
#include "rhi/interface/IDescriptorTable.hpp"
#include "rhi/interface/IEventQuery.hpp"
#include "rhi/interface/IFrameBuffer.hpp"
#include "rhi/interface/IGraphicsPipeline.hpp"
#include "rhi/interface/IHeap.hpp"
#include "rhi/interface/IInputLayout.hpp"
#include "rhi/interface/IMeshletPipeline.hpp"
#include "rhi/interface/IMessageCallback.hpp"
#include "rhi/interface/ISampler.hpp"
#include "rhi/interface/IShader.hpp"
#include "rhi/interface/ITexture.hpp"
#include "rhi/interface/ITimerQuery.hpp"
#include "rhi/interface/rt/IAccelStruct.hpp"
#include "rhi/interface/rt/IOpacityMicromap.hpp"
#include "rhi/interface/rt/IPipeline.hpp"

namespace slug::rhi
{
class IDevice;

class ICommandList : public IResource 
{
public:
    // Open / Close
    virtual void Open() = 0;
    virtual void Close() = 0;

    // Clear State
    virtual void ClearState() = 0;

    // Texture
    virtual void ClearTextureFloat(ITexture* t, TextureSubresourceSet subresources, const Color& clear_color) = 0;
    virtual void ClearDepthStencilTexture(ITexture* t, TextureSubresourceSet subresources, bool clear_depth, float depth, bool clear_stencil, uint8_t stencil) = 0;
    virtual void ClearTextureUint(ITexture* t, TextureSubresourceSet subresources, uint32_t clear_color) = 0;
    virtual void CopyTexture(ITexture* dest, const TextureSlice& dest_slice, ITexture* src, const TextureSlice& src_slice) = 0;
    virtual void CopyTexture(IStagingTexture* dest, const TextureSlice& dest_slice, ITexture* src, const TextureSlice& src_slice) = 0;
    virtual void CopyTexture(ITexture* dest, const TextureSlice& dest_slice, IStagingTexture* src, const TextureSlice& src_slice) = 0;
    virtual void WriteTexture(ITexture* dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t row_pitch, size_t depth_pitch = 0) = 0;
    virtual void ResolveTexture(ITexture* dest, const TextureSubresourceSet& dstSubresources, ITexture* src, const TextureSubresourceSet& srcSubresources) = 0;

    // Buffer
    virtual void WriteBuffer(IBuffer* b, const void* data, size_t data_size, uint64_t dest_offset_bytes = 0) = 0;
    virtual void ClearBufferUInt(IBuffer* b, uint32_t clear_value) = 0;
    virtual void CopyBuffer(IBuffer* dest, uint64_t dest_offset_bytes, IBuffer* src, uint64_t src_offset_bytes, uint64_t data_size_bytes) = 0;

    // Constants
    virtual void SetPushConstants(const void* data, size_t bytesize_t) = 0;

    // GraphicsPipeline
    virtual void SetGraphicsState(const GraphicsState& state) = 0;
    virtual void Draw(const DrawArguments& args) = 0;
    virtual void DrawIndexed(const DrawArguments& args) = 0;
    virtual void DrawIndirect(uint32_t offsetBytes, uint32_t drawCount = 1) = 0;
    virtual void DrawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount = 1) = 0;

    // ComputePipeline
    virtual void SetComputeState(const ComputeState& state) = 0;
    virtual void Dispatch(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) = 0;
    virtual void DispatchIndirect(uint32_t offsetBytes) = 0;

    // MeshletPipeline
    virtual void SetMeshletState(const MeshletState& state) = 0;
    virtual void DispatchMesh(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) = 0;

    // RaytracingPipeline
    virtual void SetRayTracingState(const rt::State& state) = 0;
    virtual void DispatchRays(const rt::DispatchRaysArguments& args) = 0;

    // Build BVH
    virtual void BuildOpacityMicromap(rt::IOpacityMicromap* omm, const rt::OpacityMicromapDesc& desc) = 0;
    virtual void BuildBottomLevelAccelStruct(rt::IAccelStruct* as, const rt::GeometryDesc* pGeometries, size_t numGeometries, rt::AccelStructBuildFlagsBitset buildFlags = rt::AccelStructBuildFlags::None) = 0;
    virtual void CompactBottomLevelAccelStructs() = 0;
    virtual void BuildTopLevelAccelStruct(rt::IAccelStruct* as, const rt::InstanceDesc* pInstances, size_t numInstances, rt::AccelStructBuildFlagsBitset buildFlags = rt::AccelStructBuildFlags::None) = 0;
    virtual void BuildTopLevelAccelStructFromBuffer(rt::IAccelStruct* as, IBuffer* instanceBuffer, uint64_t instanceBufferOffset, size_t numInstances, rt::AccelStructBuildFlagsBitset buildFlags = rt::AccelStructBuildFlags::None) = 0;

    // Timer
    virtual void BeginTimerQuery(ITimerQuery* query) = 0;
    virtual void EndTimerQuery(ITimerQuery* query) = 0;

    // Marker
    virtual void BeginMarker(core::StringView name) = 0;
    virtual void EndMarker() = 0;

    // Barrier
    virtual void SetEnableAutomaticBarriers(bool enable) = 0;

    // BindingSet
    virtual void SetResourceStatesForBindingSet(IBindingSet* bindingSet) = 0;
    virtual void SetEnableUavBarriersForTexture(ITexture* texture, bool enableBarriers) = 0;
    virtual void SetEnableUavBarriersForBuffer(IBuffer* buffer, bool enableBarriers) = 0;
    virtual void BeginTrackingTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) = 0;
    virtual void BeginTrackingBufferState(IBuffer* buffer, ResourceStates stateBits) = 0;
    virtual void SetTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits) = 0;
    virtual void SetBufferState(IBuffer* buffer, ResourceStates stateBits) = 0;
    virtual void SetAccelStructState(rt::IAccelStruct* as, ResourceStates stateBits) = 0;
    virtual void SetPermanentTextureState(ITexture* texture, ResourceStates stateBits) = 0;
    virtual void SetPermanentBufferState(IBuffer* buffer, ResourceStates stateBits) = 0;
    virtual void CommitBarriers() = 0;

    // State
    virtual ResourceStatesBitset GetTextureSubresourceState(ITexture* texture, ArraySlice arraySlice, MipLevel mipLevel) = 0;
    virtual ResourceStatesBitset GetBufferState(IBuffer* buffer) = 0;

    // Device / CommandList
    virtual IDevice* GetDevice() = 0;
    virtual const CommandListParameters& GetDesc() = 0;
    virtual CommandListState GetCurrentState() = 0;
public:

    void SetResourceStatesForFrameBuffer(IFrameBuffer* framebuffer)
    {
        const FrameBufferDesc& desc = framebuffer->GetDesc();

        for (const auto& attachment : desc.colorAttachments)
        {
            SetTextureState(attachment.texture, attachment.subresources, ResourceStates::RenderTarget);
        }

        if (desc.depthAttachment.Valid())
        {
            SetTextureState(desc.depthAttachment.texture, desc.depthAttachment.subresources, desc.depthAttachment.readOnly ? ResourceStates::DepthRead : ResourceStates::DepthWrite);
        }
    }

};
using CommandListPtr = core::TReferencePtr<ICommandList>;
}
