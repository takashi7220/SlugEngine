#pragma once

#include "rhi/common/StateTracking.hpp"
#include "rhi/Rhi.hpp"
namespace slug
{
namespace rhi
{
CommandListResourceStateTracker::CommandListResourceStateTracker()
{}

CommandListResourceStateTracker::~CommandListResourceStateTracker() 
{}

// Buffer

void CommandListResourceStateTracker::SetEnableUavBarriersForBuffer(BufferStateExtension* buffer, bool enableBarriers)
{
    BufferState* bufferState = this->GetBufferStateTracking(buffer, true);

    bufferState->enableUavBarriers = enableBarriers;
    bufferState->firstUavBarrierPlaced = false;
}

void CommandListResourceStateTracker::BeginTrackingBufferState(BufferStateExtension* buffer, ResourceStatesBitset stateBits)
{
    BufferState* textureState = this->GetBufferStateTracking(buffer, true);
    textureState->state = stateBits;
}

void CommandListResourceStateTracker::EndTrackingBufferState(BufferStateExtension* buffer, ResourceStatesBitset stateBits, bool permanent)
{
    this->RequireBufferState(buffer, stateBits);

    if (permanent) 
    {
        m_permanentBufferStates.push_back(core::MakePair(buffer, stateBits));
    }
}

ResourceStatesBitset CommandListResourceStateTracker::GetBufferState(BufferStateExtension* buffer)
{
    BufferState* bufferState = this->GetBufferStateTracking(buffer, false);

    if (!bufferState)
    {
        ResourceStatesBitset stateBits = {};
        return stateBits;
    }

    return bufferState->state;
}

void CommandListResourceStateTracker::RequireBufferState(BufferStateExtension* buffer, ResourceStatesBitset state)
{
    if (buffer->descRef.volatileBuffer) 
    {
        return;
    }

    if (!buffer->permanentState.Any())
    {
        return;
    }

    if (buffer->descRef.cpuAccess != CpuAccessMode::None) 
    {
        return;
    }

    SLUG_MAYBE_UNUSED BufferState* bufferState = GetBufferStateTracking(buffer, true);

#if defined(DEBUG_MODE)
    if (!bufferState->state.Any())
    {
        core::String msg = {};
        core::StringUtility::Sprintfs(msg, "The buffer's state is unknown. %s\n", buffer->descRef.debugName.c_str());
        core::StringUtility::Sprintfs(msg, "Call CommandList::beginTrackingBufferState() before using the buffer,\n");
        core::StringUtility::Sprintfs(msg, "or use keepInitialState and initialState in BufferDesc.\n");
        SLUG_LOG(RHILogCategory, core::LogType::Error, msg.c_str());
    }
#endif
}

void CommandListResourceStateTracker::KeepBufferInitialStates()
{
    for (auto bufferState = m_bufferStates.begin(); bufferState != m_bufferStates.end(); bufferState++) 
    {
        if (bufferState->first &&
            bufferState->first->descRef.keepInitialState &&
            bufferState->second &&
            bufferState->second->permanentTransition) 
        {
            RequireBufferState(bufferState->first, bufferState->first->descRef.initialState);
        }
    }
}

const core::TVector<BufferBarrier>& CommandListResourceStateTracker::GetBufferBarriers() const
{
    return m_bufferBarriers;
}


// Texture

void CommandListResourceStateTracker::SetEnableUavBarriersForTexture(TextureStateExtension* texture, bool enableBarriers)
{
    TextureState* textureState = this->GetTextureStateTracking(texture, true);

    textureState->enableUavBarriers = enableBarriers;
    textureState->firstUavBarrierPlaced = false;
}

void CommandListResourceStateTracker::BeginTrackingTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStatesBitset stateBits) 
{
    const TextureDesc& desc = texture->descRef;

    TextureState* textureState = GetTextureStateTracking(texture, true);

    subresources = subresources.Resolve(desc, false);

    if (subresources.CheckEntireTexture(desc)) 
    {
        textureState->state = stateBits;
        textureState->subresourceStates.clear();
    }
    else 
    {
        textureState->subresourceStates.resize(desc.mipLevels * desc.arraySize, textureState->state);
        textureState->state.Reset();

        for (MipLevel mipLevel = subresources.baseMipLevel; mipLevel < subresources.baseMipLevel + subresources.numMipLevels; mipLevel++) 
        {
            for (ArraySlice arraySlice = subresources.baseArraySlice; arraySlice < subresources.baseArraySlice + subresources.numArraySlices; arraySlice++) 
            {
                uint32_t subresource = CalcSubresource(mipLevel, arraySlice, desc);
                textureState->subresourceStates[subresource] = stateBits;
            }
        }
    }
}

void CommandListResourceStateTracker::EndTrackingTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStatesBitset stateBits, bool permanent) 
{
    const TextureDesc& desc = texture->descRef;
    subresources = subresources.Resolve(desc, false);

    if (permanent && !subresources.CheckEntireTexture(desc)) 
    {

        core::String msg = {};
        core::StringUtility::Sprintfs(msg, "Attempted to perform a permanent state transition on a subset of subresources of texture ");
        core::StringUtility::Sprintfs(msg, "%s\n", desc.debugName.c_str());
        SLUG_LOG(RHILogCategory, core::LogType::Error, msg.c_str());
        permanent = false;
    }

    RequireTextureState(texture, subresources, stateBits);

    if (permanent) 
    {
        m_permanentTextureStates.push_back(core::MakePair(texture, stateBits));
        GetTextureStateTracking(texture, true)->permanentTransition = true;
    }
}

ResourceStatesBitset CommandListResourceStateTracker::GetTextureSubresourceState(TextureStateExtension* texture, ArraySlice arraySlice, MipLevel mipLevel) 
{
    TextureState* textureState = GetTextureStateTracking(texture, false);

    if (!textureState)
    {
        ResourceStatesBitset stateBits = {};
        return stateBits;
    }

    uint32_t subresource = CalcSubresource(mipLevel, arraySlice, texture->descRef);
    return textureState->subresourceStates[subresource];
}

void CommandListResourceStateTracker::RequireTextureState(TextureStateExtension* texture, TextureSubresourceSet subresources, ResourceStatesBitset state) 
{
    if (texture->permanentState.Any()) 
    {
        VerifyPermanentResourceState(texture->permanentState, state, true, texture->descRef.debugName);
        return;
    }

    subresources = subresources.Resolve(texture->descRef, false);

    TextureState* tracking = GetTextureStateTracking(texture, true);

    if (subresources.CheckEntireTexture(texture->descRef) && tracking->subresourceStates.empty()) 
    {

        bool transitionNecessary = tracking->state != state;
        bool uavNecessary = state.Test(ResourceStates::UnorderedAccess);
        uavNecessary &= (tracking->enableUavBarriers || !tracking->firstUavBarrierPlaced);

        if (transitionNecessary || uavNecessary) 
        {
            TextureBarrier barrier;
            barrier.texture = texture;
            barrier.entireTexture = true;
            barrier.stateBefore = tracking->state;
            barrier.stateAfter = state;
            m_textureBarriers.push_back(barrier);
        }

        tracking->state = state;

        if (uavNecessary && !transitionNecessary) 
        {
            tracking->firstUavBarrierPlaced = true;
        }
    }
    else 
    {
        bool stateExpanded = false;
        if (tracking->subresourceStates.empty()) 
        {
            if (!tracking->state.Any()) 
            {
                core::String msg = {};
                core::StringUtility::Sprintfs(msg, "Unknown prior state of texture %s", texture->descRef.debugName.c_str());
                core::StringUtility::Sprintfs(msg, "Call CommandList::beginTrackingTextureState(...) before using the texture or use the ");
                core::StringUtility::Sprintfs(msg, "keepInitialState and initialState members of TextureDesc.");
                SLUG_LOG(RHILogCategory, core::LogType::Error, msg.c_str());
            }

            tracking->subresourceStates.resize(texture->descRef.mipLevels * texture->descRef.arraySize, tracking->state);
            tracking->state.Reset();
            stateExpanded = true;
        }

        bool anyUavBarrier = false;

        for (ArraySlice arraySlice = subresources.baseArraySlice; arraySlice < subresources.baseArraySlice + subresources.numArraySlices; arraySlice++) 
        {
            for (MipLevel mipLevel = subresources.baseMipLevel; mipLevel < subresources.baseMipLevel + subresources.numMipLevels; mipLevel++) 
            {
                uint32_t subresourceIndex = CalcSubresource(mipLevel, arraySlice, texture->descRef);

                auto priorState = tracking->subresourceStates[subresourceIndex];

                if (priorState.Test(ResourceStates::Unknown) && !stateExpanded) 
                {
                    core::String msg = {};
                    core::StringUtility::Sprintfs(msg, "Unknown prior state of texture %s", texture->descRef.debugName.c_str());
                    core::StringUtility::Sprintfs(msg, "subresource (MipLevel = %d, ArraySlice = %d", mipLevel, arraySlice);
                    core::StringUtility::Sprintfs(msg, "Call CommandList::beginTrackingTextureState(...) before using the texture or use the ");
                    core::StringUtility::Sprintfs(msg, "keepInitialState and initialState members of TextureDesc.");
                    SLUG_LOG(RHILogCategory, core::LogType::Error, msg.c_str());
                }

                bool transitionNecessary = priorState != state;
                bool uavNecessary = state.Test(ResourceStates::UnorderedAccess);
                uavNecessary &= !anyUavBarrier && (tracking->enableUavBarriers || !tracking->firstUavBarrierPlaced);

                if (transitionNecessary || uavNecessary) 
                {
                    TextureBarrier barrier;
                    barrier.texture = texture;
                    barrier.entireTexture = false;
                    barrier.mipLevel = mipLevel;
                    barrier.arraySlice = arraySlice;
                    barrier.stateBefore = priorState;
                    barrier.stateAfter = state;
                    m_textureBarriers.push_back(barrier);
                }

                tracking->subresourceStates[subresourceIndex] = state;

                if (uavNecessary && !transitionNecessary) 
                {
                    anyUavBarrier = true;
                    tracking->firstUavBarrierPlaced = true;
                }
            }
        }
    }
}

void CommandListResourceStateTracker::KeepTextureInitialStates() 
{
    for (auto textureState = m_textureStates.begin(); textureState != m_textureStates.end(); textureState++) 
    {
        bool requireTextureState = textureState->first->descRef.keepInitialState;
        requireTextureState &= !textureState->first->permanentState.Any();
        requireTextureState &= !textureState->second->permanentTransition;
        if (requireTextureState) 
        {
            RequireTextureState(textureState->first, AllSubresources, textureState->first->descRef.initialState);
        }
    }
}

const core::TVector<TextureBarrier>& CommandListResourceStateTracker::GetTextureBarriers() const
{
    return m_textureBarriers;
}

void CommandListResourceStateTracker::CommandListSubmitted() 
{
    for (auto textureState = m_permanentTextureStates.begin(); textureState != m_permanentTextureStates.end(); textureState++) 
    {
        if (textureState->first->permanentState.Any() && textureState->first->permanentState != textureState->second) 
        {
            continue;
        }
        textureState->first->permanentState = textureState->second;
    }
    m_permanentTextureStates.clear();

    for (auto bufferState = m_permanentBufferStates.begin(); bufferState != m_permanentBufferStates.end(); bufferState++) 
    {
        if (bufferState->first->permanentState.Any() && bufferState->first->permanentState != bufferState->second)
        {
            continue;
        }
        bufferState->first->permanentState = bufferState->second;
    }
    m_permanentBufferStates.clear();

    for (auto textureState = m_textureStates.begin(); textureState != m_textureStates.end(); textureState++) 
    {
        if (textureState->first->descRef.keepInitialState && !textureState->first->stateInitialized)
        {
            textureState->first->stateInitialized = true;
        }
    }

    m_textureStates.clear();
    m_bufferStates.clear();
}

void CommandListResourceStateTracker::ClearBarriers()
{
    m_textureBarriers.clear();
    m_bufferBarriers.clear();
}

TextureState* CommandListResourceStateTracker::GetTextureStateTracking(TextureStateExtension* texture, bool allowCreate) 
{
    auto textureStateItr = m_textureStates.find(texture);
    if (textureStateItr != m_textureStates.end())
    {
        return textureStateItr->second.get();
    }

    if (!allowCreate) 
    {
        return nullptr;
    }

    core::TUniquePtr<TextureState> textureStatePtr = core::MakeUnique<TextureState>();
    TextureState* textureState = textureStatePtr.get();
    m_textureStates.insert(core::MakePair(texture, textureState));

    if (texture->descRef.keepInitialState) 
    {
        if(texture->stateInitialized)
        { 
            textureState->state = texture->descRef.initialState;
        }
        else 
        {
            textureState->state.Reset();
            textureState->state.Set(ResourceStates::Common);
        }
    }

    return textureState;
}

BufferState* CommandListResourceStateTracker::GetBufferStateTracking(BufferStateExtension* buffer, bool allowCreate) 
{
    auto bufferStateItr = m_bufferStates.find(buffer);
    if (bufferStateItr != m_bufferStates.end())
    {
        return bufferStateItr->second.get();
    }

    if (!allowCreate)
    {
        return nullptr;
    }

    core::TUniquePtr<BufferState> bufferStatePtr = core::MakeUnique<BufferState>();
    BufferState* bufferState = bufferStatePtr.get();
    m_bufferStates.insert(core::MakePair(buffer, bufferState));

    if (buffer->descRef.keepInitialState)
    {
        bufferState->state = buffer->descRef.initialState;
    }

    return bufferState;
}

bool CommandListResourceStateTracker::VerifyPermanentResourceState(ResourceStatesBitset permanentState, ResourceStatesBitset requiredState, bool isTexture, const core::String& debugName) 
{
    if ((permanentState & requiredState) != requiredState) 
    {
        core::String msg = {};
        core::StringUtility::Sprintfs(msg, "Permanent %s, %s", isTexture ? "texture" : "buffer", debugName.c_str());
        core::StringUtility::Sprintfs(msg, "doesn't have the right state bits. Required: %s", requiredState.ToString());
        core::StringUtility::Sprintfs(msg, ", present: %s", permanentState.ToString());
        SLUG_LOG(RHILogCategory, core::LogType::Error, msg.c_str());
        return false;
    }

    return true;
}

uint32_t CommandListResourceStateTracker::CalcSubresource(MipLevel mipLevel, ArraySlice arraySlice, const TextureDesc& desc)
{
    return mipLevel + arraySlice * desc.mipLevels;
}

}
}
