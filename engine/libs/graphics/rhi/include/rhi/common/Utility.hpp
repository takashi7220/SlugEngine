#pragma once

#include "rhi/Rhi.hpp"

namespace slug::rhi
{

constexpr uint64_t c_VersionSubmittedFlag = 0x8000000000000000;
constexpr uint32_t c_VersionQueueShift = 60;
constexpr uint32_t c_VersionQueueMask = 0x7;
constexpr uint64_t c_VersionIDMask = 0x0FFFFFFFFFFFFFFF;

constexpr uint64_t MakeVersion(uint64_t id, CommandQueue queue, bool submitted)
{
    uint64_t result = (id & c_VersionIDMask) | (uint64_t(queue) << c_VersionQueueShift);
    if (submitted) result |= c_VersionSubmittedFlag;
    return result;
}

constexpr uint64_t VersionGetInstance(uint64_t version)
{
    return version & c_VersionIDMask;
}

constexpr CommandQueue VersionGetQueue(uint64_t version)
{
    return CommandQueue((version >> c_VersionQueueShift) & c_VersionQueueMask);
}

constexpr bool VersionGetSubmitted(uint64_t version)
{
    return (version & c_VersionSubmittedFlag) != 0;
}

template<typename T> T TAlign(T size, T alignment)
{
    return (size + alignment - 1) & ~(alignment - 1);
}

template<typename T, typename U> bool TArraysAreDifferent(const T& a, const U& b)
{
    if (a.size() != b.size())
    {
        return true;
    }

    for (uint32_t i = 0; i < uint32_t(a.size()); i++)
    {
        if (a[i] != b[i])
            return true;
    }

    return false;
}

template<typename T, typename U> uint32_t ArrayDifferenceMask(const T& a, const U& b)
{
    SLUG_ASSERT(a.size() <= 32);
    SLUG_ASSERT(b.size() <= 32);

    if (a.size() != b.size())
    {
        return ~0u;
    }

    uint32_t mask = 0;
    for (uint32_t i = 0; i < uint32_t(a.size()); i++)
    {
        if (a[i] != b[i])
        {
            mask |= (1 << i);
        }
    }

    return mask;
}

class Utility 
{
public:
    static core::String NumericTypeToString(uint32_t value);
    static core::String NumericTypeToString(int64_t value);
    static core::String NumericTypeToString(int32_t value);
    static core::String NumericTypeToString(size_t value);

    static void ConvertFormatToInfo(FormatType type, uint32_t& elementNum, uint32_t& valueByteSize, core::String& name);
    static BlendState::RenderTarget CreateAddBlendState(BlendFactor srcBlend, BlendFactor dstBlend);
    static rhi::BufferDesc CreateStaticConstantBufferDesc(uint32_t byteSize, const char* debugName);
    static BufferDesc CreateVolatileConstantBufferDesc(uint32_t byteSize, const char* debugName, uint32_t maxVersions);
    static bool CreateBindingSetAndLayout(IDevice* device, rhi::ShaderType visibility, uint32_t registerSpace, const BindingSetDesc& bindingSetDesc, BindingLayoutPtr& bindingLayout, BindingSetPtr& bindingSet);

    static void ClearColorAttachment(ICommandList* commandList, IFrameBuffer* framebuffer, uint32_t attachmentIndex, Color color);
    static void ClearDepthStencilAttachment(ICommandList* commandList, IFrameBuffer* framebuffer, float depth, uint32_t stencil);

    static void BuildBottomLevelAccelStruct(ICommandList* commandList, rt::IAccelStruct* as, const rt::AccelStructDesc& desc);

    static void TextureUavBarrier(ICommandList* commandList, ITexture* texture);

    static void BufferUavBarrier(ICommandList* commandList, IBuffer* buffer);

    static FormatType ChooseFormat(IDevice* device, rhi::FormatSupport requiredFeatures, const rhi::FormatType* requestedFormats, size_t requestedFormatCount);

    static const char* GraphicApiToString(GraphicsApiType api);
    static const char* TextureDimensionToString(TextureDimension dimension);
    static const char* DebugNameToString(const core::String& debugName);
    static const char* ShaderStageToString(ShaderType stage);
    static const char* ResourceTypeToString(ResourceType type);
    static const char* FormatToString(FormatType format);
    static const char* CommandQueueToString(CommandQueue queue);

    static core::String GenerateHeapDebugName(const HeapDesc& desc);
    static core::String GenerateTextureDebugName(const TextureDesc& desc);
    static core::String GenerateBufferDebugName(const BufferDesc& desc);

    static ResourceType GetNormalizedResourceType(ResourceType type);
    static bool AreResourceTypesCompatible(ResourceType a, ResourceType b);
};

}
