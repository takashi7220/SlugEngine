#pragma once

#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/memory/UniquePtr.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/ITexture.hpp"
#include "rhi/interface/IMessageCallback.hpp"

namespace slug 
{
namespace rhi 
{

struct BufferStateExtension
{
    const BufferDesc& descRef;
    ResourceStatesBitset permanentState = {};

    explicit BufferStateExtension(const BufferDesc& desc) 
        : descRef(desc)
    {}
};

struct BufferState
{
    ResourceStatesBitset state = {};
    bool enableUavBarriers = true;
    bool firstUavBarrierPlaced = false;
    bool permanentTransition = false;
};

struct BufferBarrier
{
    BufferStateExtension* buffer = nullptr;
    ResourceStatesBitset stateBefore = {};
    ResourceStatesBitset stateAfter = {};
};

struct TextureStateExtension
{
    const TextureDesc& descRef;
    ResourceStatesBitset permanentState = {};
    bool stateInitialized = false;

    explicit TextureStateExtension(const TextureDesc& desc)
        : descRef(desc)
    {}
};

struct TextureState 
{
    core::TVector<ResourceStatesBitset> subresourceStates;
    ResourceStatesBitset state = {};
    bool enableUavBarriers = true;
    bool firstUavBarrierPlaced = false;
    bool permanentTransition = false;
};

struct TextureBarrier 
{
    TextureStateExtension* texture = nullptr;
    MipLevel mipLevel = 0;
    ArraySlice arraySlice = 0;
    bool entireTexture = false;
    ResourceStatesBitset stateBefore = {};
    ResourceStatesBitset stateAfter = {};
};

class CommandListResourceStateTracker
{
public:
    explicit CommandListResourceStateTracker();
    virtual ~CommandListResourceStateTracker();

    // Buffer
    void SetEnableUavBarriersForBuffer(BufferStateExtension* buffer, bool enableBarriers);
    void BeginTrackingBufferState(BufferStateExtension* buffer, ResourceStatesBitset stateBits);
    void EndTrackingBufferState(BufferStateExtension* buffer, ResourceStatesBitset stateBits, bool permanent);
    ResourceStatesBitset GetBufferState(BufferStateExtension* buffer);
    void RequireBufferState(BufferStateExtension* buffer, ResourceStatesBitset state);
    void KeepBufferInitialStates();
    const core::TVector<BufferBarrier>& GetBufferBarriers() const;

    // Texture
    void SetEnableUavBarriersForTexture(TextureStateExtension* texture, bool enableBarriers);
    void BeginTrackingTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStatesBitset stateBits);
    void EndTrackingTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStatesBitset stateBits, bool permanent);
    ResourceStatesBitset GetTextureSubresourceState(TextureStateExtension* texture, ArraySlice arraySlice, MipLevel mipLevel);
    void RequireTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStatesBitset state);
    void KeepTextureInitialStates();
    const core::TVector<TextureBarrier>& GetTextureBarriers() const;

    void CommandListSubmitted();
    void ClearBarriers();

    static bool VerifyPermanentResourceState(ResourceStatesBitset permanentState, ResourceStatesBitset requiredState, bool isTexture, const core::String& debugName);
private:
    TextureState* GetTextureStateTracking(TextureStateExtension* texture, bool allowCreate);
    BufferState* GetBufferStateTracking(BufferStateExtension* buffer, bool allowCreate);

    static uint32_t CalcSubresource(MipLevel mipLevel, ArraySlice arraySlice, const TextureDesc& desc);
private:

    core::TUnorderedMap<TextureStateExtension*, core::TUniquePtr<TextureState>> m_textureStates;
    core::TUnorderedMap<BufferStateExtension*, core::TUniquePtr<BufferState>> m_bufferStates;

    core::TVector<core::TPair<TextureStateExtension*, ResourceStatesBitset>> m_permanentTextureStates;
    core::TVector<core::TPair<BufferStateExtension*, ResourceStatesBitset >> m_permanentBufferStates;

    core::TVector<TextureBarrier> m_textureBarriers;
    core::TVector<BufferBarrier> m_bufferBarriers;
};
}
}
