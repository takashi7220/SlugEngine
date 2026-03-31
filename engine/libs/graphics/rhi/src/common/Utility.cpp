#include "rhi/common/Utility.hpp"

namespace slug
{
namespace rhi
{

struct ConvertFormatInfo
{
    FormatType type;
    uint32_t elementNum;
    uint32_t valueByteSize;
    core::String name;
};

static const ConvertFormatInfo CONVERT_FORMAT_INFO_TABLE[] =
{
    {FormatType::UNKNOWN,           0, 0, "Unknown"},
    {FormatType::R8_UINT,           1, 1, "R8_UINT"},
    {FormatType::R8_SINT,           1, 1, "R8_SINT"},
    {FormatType::R8_UNORM,          1, 1, "R8_UNORM"},
    {FormatType::R8_SNORM,          1, 1, "R8_SNORM"},
    {FormatType::RG8_UINT,          2, 2, "RG8_UINT"},
    {FormatType::RG8_SINT,          2, 2, "RG8_SINT"},
    {FormatType::RG8_UNORM,         2, 2, "RG8_UNORM"},
    {FormatType::RG8_SNORM,         2, 2, "RG8_SNORM"},
    {FormatType::R16_UINT,          1, 2, "R16_UINT"},
    {FormatType::R16_SINT,          1, 2, "R16_SINT"},
    {FormatType::R16_UNORM,         1, 2, "R16_UNORM"},
    {FormatType::R16_SNORM,         1, 2, "R16_SNORM"},
    {FormatType::R16_FLOAT,         1, 2, "R16_FLOAT"},
    {FormatType::RGBA8_UINT,        4, 4, "RGBA8_UINT"},
    {FormatType::RGBA8_SINT,        4, 4, "RGBA8_SINT"},
    {FormatType::RGBA8_UNORM,       4, 4, "RGBA8_UNORM"},
    {FormatType::RGBA8_SNORM,       4, 4, "RGBA8_SNORM"},
    {FormatType::BGRA8_UNORM,       4, 4, "BGRA8_UNORM"},
    {FormatType::SRGBA8_UNORM,      4, 4, "SRGBA8_UNORM"},
    {FormatType::SBGRA8_UNORM,      4, 4, "SBGRA8_UNORM"},
    {FormatType::R10G10B10A2_UNORM, 4, 4, "R10G10B10A2_UNORM"},
    {FormatType::R11G11B10_FLOAT,   3, 4, "R11G11B10_FLOAT"},
    {FormatType::RG16_UINT,         2, 4, "RG16_UINT"},
    {FormatType::RG16_SINT,         2, 4, "RG16_SINT"},
    {FormatType::RG16_UNORM,        2, 4, "RG16_UNORM"},
    {FormatType::RG16_SNORM,        2, 4, "RG16_SNORM"},
    {FormatType::RG16_FLOAT,        2, 4, "RG16_FLOAT"},
    {FormatType::R32_UINT,          1, 4, "R32_UINT"},
    {FormatType::R32_SINT,          1, 4, "R32_SINT"},
    {FormatType::R32_FLOAT,         1, 4, "R32_FLOAT"},
    {FormatType::RGBA16_UINT,       4, 8, "RGBA16_UINT"},
    {FormatType::RGBA16_SINT,       4, 8, "RGBA16_SINT"},
    {FormatType::RGBA16_FLOAT,      4, 8, "RGBA16_FLOAT"},
    {FormatType::RGBA16_UNORM,      4, 8, "RGBA16_UNORM"},
    {FormatType::RGBA16_SNORM,      4, 8, "RGBA16_SNORM"},
    {FormatType::RG32_UINT,         2, 8, "RG32_UINT"},
    {FormatType::RG32_SINT,         2, 8, "RG32_SINT"},
    {FormatType::RG32_FLOAT,        2, 8, "RG32_FLOAT"},
    {FormatType::RGB32_UINT,        3, 12, "RGB32_UINT"},
    {FormatType::RGB32_SINT,        3, 12, "RGB32_SINT"},
    {FormatType::RGB32_FLOAT,       3, 12, "RGB32_FLOAT"},
    {FormatType::RGBA32_UINT,       4, 16, "RGBA32_UINT"},
    {FormatType::RGBA32_SINT,       4, 16, "RGBA32_SINT"},
    {FormatType::RGBA32_FLOAT,      4, 16, "RGBA32_FLOAT"},
    {FormatType::D16,               1, 2, "D16"},
    {FormatType::D24S8,             2, 4, "D24S8"},
    {FormatType::X24G8_UINT,        2, 4, "X24G8_UINT"},
    {FormatType::D32,               1, 4, "D32"},
    {FormatType::D32S8,             2, 5, "D32S8"},
    {FormatType::X32G8_UINT,        2, 5, "X32G8_UINT"}
};

static void ConvertSetToLayout(const BindingSetItemArray& setDesc, BindingLayoutItemArray& layoutDesc)
{
    for (auto& item : setDesc)
    {
        BindingLayoutItem layout_item{};
        layout_item.slot = item.slot;
        layout_item.type = item.type;
        if (item.type == ResourceType::PushConstants)
        {
            layout_item.size = uint16_t(item.range.byteSize);
        }
        layoutDesc.push_back(layout_item);
    }
}

core::String Utility::NumericTypeToString(uint32_t value)
{
    core::StringStream ss = {};
    ss << value;
    return ss.str().c_str();
}


core::String Utility::NumericTypeToString(int64_t value)
{
    core::StringStream ss = {};
    ss << value;
    return ss.str().c_str();
}

core::String Utility::NumericTypeToString(int32_t value)
{
    core::StringStream ss = {};
    ss << value;
    return ss.str().c_str();
}

core::String Utility::NumericTypeToString(size_t value)
{
    core::StringStream ss = {};
    ss << value;
    return ss.str().c_str();
}

void Utility::ConvertFormatToInfo(FormatType type, uint32_t& elementNum, uint32_t& valueByteSize, core::String& name)
{
    for (auto& info : CONVERT_FORMAT_INFO_TABLE)
    {
        if (info.type == type)
        {
            elementNum = info.elementNum;
            valueByteSize = info.valueByteSize;
            name = info.name;
        }
    }
}

BlendState::RenderTarget Utility::CreateAddBlendState(BlendFactor srcBlend, BlendFactor dstBlend)
{
    BlendState::RenderTarget target;
    target.blendEnable = true;
    target.blendOp = BlendOp::Add;
    target.srcBlend = srcBlend;
    target.destBlend = dstBlend;
    target.srcBlendAlpha = BlendFactor::Zero;
    target.destBlendAlpha = BlendFactor::One;
    return target;
}

rhi::BufferDesc Utility::CreateStaticConstantBufferDesc(uint32_t byteSize, const char* debugName)
{
    BufferDesc constantBufferDesc;
    constantBufferDesc.byteSize = byteSize;
    constantBufferDesc.debugName = debugName;
    constantBufferDesc.useConstantBuffer = true;
    constantBufferDesc.volatileBuffer = false;
    return constantBufferDesc;
}

BufferDesc Utility::CreateVolatileConstantBufferDesc(uint32_t byteSize, const char* debugName, uint32_t maxVersions)
{
    BufferDesc constantBufferDesc;
    constantBufferDesc.byteSize = byteSize;
    constantBufferDesc.debugName = debugName;
    constantBufferDesc.useConstantBuffer = true;
    constantBufferDesc.volatileBuffer = true;
    constantBufferDesc.maxVersions = maxVersions;
    return constantBufferDesc;
}

bool Utility::CreateBindingSetAndLayout(IDevice* device, rhi::ShaderType visibility, uint32_t registerSpace, const BindingSetDesc& bindingSetDesc, BindingLayoutPtr& bindingLayout, BindingSetPtr& bindingSet)
{
    if (!bindingLayout)
    {
        BindingLayoutDesc binding_layout_desc;
        binding_layout_desc.visibility = visibility;
        binding_layout_desc.registerSpace = registerSpace;
        ConvertSetToLayout(bindingSetDesc.bindings, binding_layout_desc.bindings);

        bindingLayout = device->CreateBindingLayout(binding_layout_desc);

        if (!bindingLayout)
        {
            return false;
        }
    }

    if (!bindingSet)
    {
        bindingSet = device->CreateBindingSet(bindingSetDesc, bindingLayout);

        if (!bindingSet)
        {
            return false;
        }
    }

    return true;
}

void Utility::ClearColorAttachment(ICommandList* commandList, IFrameBuffer* framebuffer, uint32_t attachmentIndex, Color color)
{
    const FrameBufferAttachment& att = framebuffer->GetDesc().colorAttachments[attachmentIndex];
    if (att.texture) 
    {
        commandList->ClearTextureFloat(att.texture, att.subresources, color);
    }
}

void Utility::ClearDepthStencilAttachment(ICommandList* commandList, IFrameBuffer* framebuffer, float depth, uint32_t stencil) 
{
    const FrameBufferAttachment& att = framebuffer->GetDesc().depthAttachment;
    if (att.texture) 
    {
        commandList->ClearTextureFloat(att.texture, att.subresources, Color(depth, float(stencil), 0.f, 0.f));
    }
}

void Utility::BuildBottomLevelAccelStruct(ICommandList* commandList, rt::IAccelStruct* as, const rt::AccelStructDesc& desc)
{
    commandList->BuildBottomLevelAccelStruct(as, desc.bottomLevelGeometries.data(), desc.bottomLevelGeometries.size(), desc.buildFlags);
}

void Utility::TextureUavBarrier(ICommandList* commandList, ITexture* texture) 
{
    commandList->SetTextureState(texture, AllSubresources, ResourceStates::UnorderedAccess);
}

void Utility::BufferUavBarrier(ICommandList* commandList, IBuffer* buffer)
{
    commandList->SetBufferState(buffer, ResourceStates::UnorderedAccess);
}

FormatType Utility::ChooseFormat(IDevice* device, rhi::FormatSupport requiredFeatures, const rhi::FormatType* requestedFormats, size_t requestedFormatCount)
{
    SLUG_ASSERT(device);
    SLUG_ASSERT(requestedFormats || requestedFormatCount == 0);

    for (size_t i = 0; i < requestedFormatCount; i++) 
    {
        if ((device->QueryFormatSupport(requestedFormats[i]).Test(requiredFeatures)))
        {
            return requestedFormats[i];
        }
    }

    return FormatType::UNKNOWN;
}

const char* Utility::GraphicApiToString(GraphicsApiType api)
{
    switch (api) {
    case GraphicsApiType::D3D11:
        return "D3D11";
    case GraphicsApiType::D3D12:
        return "D3D12";
    case GraphicsApiType::VULKAN:
        return "Vulkan";
    default:
        return "<UNKNOWN>";
    }
}
const char* Utility::TextureDimensionToString(TextureDimension dimension)
{
    switch (dimension) {
    case TextureDimension::Texture1D:
        return "Texture1D";
    case TextureDimension::Texture1DArray:
        return "Texture1DArray";
    case TextureDimension::Texture2D:
        return "Texture2D";
    case TextureDimension::Texture2DArray:
        return "Texture2DArray";
    case TextureDimension::TextureCube:
        return "TextureCube";
    case TextureDimension::TextureCubeArray:
        return "TextureCubeArray";
    case TextureDimension::Texture2DMS:
        return "Texture2DMS";
    case TextureDimension::Texture2DMSArray:
        return "Texture2DMSArray";
    case TextureDimension::Texture3D:
        return "Texture3D";
    case TextureDimension::Unknown:
        return "Unknown";
    default:
        return "<INVALID>";
    }
}

const char* Utility::DebugNameToString(const core::String& debugName)
{
    return debugName.empty() ? "<UNNAMED>" : debugName.c_str();
}

const char* Utility::ShaderStageToString(ShaderType stage)
{
    switch (stage) {
    case ShaderType::None:
        return "None";
    case ShaderType::Compute:
        return "Compute";
    case ShaderType::Vertex:
        return "Vertex";
    case ShaderType::Hull:
        return "Hull";
    case ShaderType::Domain:
        return "Domain";
    case ShaderType::Geometry:
        return "Geometry";
    case ShaderType::Pixel:
        return "Pixel";
    case ShaderType::Amplification:
        return "Amplification";
    case ShaderType::Mesh:
        return "Mesh";
    case ShaderType::AllGraphics:
        return "AllGraphics";
    case ShaderType::RayGeneration:
        return "RayGeneration";
    case ShaderType::AnyHit:
        return "AnyHit";
    case ShaderType::ClosestHit:
        return "ClosestHit";
    case ShaderType::Miss:
        return "Miss";
    case ShaderType::Intersection:
        return "Intersection";
    case ShaderType::Callable:
        return "Callable";
    case ShaderType::AllRayTracing:
        return "AllRayTracing";
    case ShaderType::All:
        return "All";
    default:
        return "<INVALID>";
    }
}

const char* Utility::ResourceTypeToString(ResourceType type)
{
    switch (type) {
    case ResourceType::None:
        return "None";
    case ResourceType::Texture_SRV:
        return "Texture_SRV";
    case ResourceType::Texture_UAV:
        return "Texture_UAV";
    case ResourceType::TypedBuffer_SRV:
        return "Buffer_SRV";
    case ResourceType::TypedBuffer_UAV:
        return "Buffer_UAV";
    case ResourceType::StructuredBuffer_SRV:
        return "StructuredBuffer_SRV";
    case ResourceType::StructuredBuffer_UAV:
        return "StructuredBuffer_UAV";
    case ResourceType::RawBuffer_SRV:
        return "RawBuffer_SRV";
    case ResourceType::RawBuffer_UAV:
        return "RawBuffer_UAV";
    case ResourceType::ConstantBuffer:
        return "ConstantBuffer";
    case ResourceType::VolatileConstantBuffer:
        return "VolatileConstantBuffer";
    case ResourceType::Sampler:
        return "Sampler";
    case ResourceType::RayTracingAccelStruct:
        return "RayTracingAccelStruct";
    case ResourceType::PushConstants:
        return "PushConstants";
    case ResourceType::Count:
    default:
        return "<INVALID>";
    }
}

const char* Utility::FormatToString(FormatType format)
{
    return GetFormatInfo(format).name;
}

const char* Utility::CommandQueueToString(CommandQueue queue)
{
    switch (queue) {
    case CommandQueue::Graphics:
        return "Graphics";
    case CommandQueue::Compute:
        return "Compute";
    case CommandQueue::Copy:
        return "Copy";
    case CommandQueue::Count:
    default:
        return "<INVALID>";
    }
}

core::String Utility::GenerateHeapDebugName(const HeapDesc& desc)
{
    core::String txt;

    txt += "Unnamed ";

    switch (desc.type) {
    case HeapType::DeviceLocal:
        txt += "DeviceLocal";
        break;
    case HeapType::Upload:
        txt += "Upload";
        break;
    case HeapType::ReadBack:
        txt += "Readback";
        break;
    default:
        txt += "Invalid-Type";
        break;
    }

    txt += " Heap (";
    txt += NumericTypeToString(desc.capacity);
    txt += " bytes)";

    return txt;
}

core::String Utility::GenerateTextureDebugName(const TextureDesc& desc)
{
    core::String txt = "";

    txt += "Unnamed ";
    txt += TextureDimensionToString(desc.dimension);
    txt += " (";
    txt += GetFormatInfo(desc.format).name;
    txt += ", Width = ";
    txt += NumericTypeToString(desc.width);

    if (desc.dimension >= TextureDimension::Texture2D) {
        txt += ", Height = ";
        txt += NumericTypeToString(desc.height);
    }

    if (desc.dimension == TextureDimension::Texture3D) {
        txt += NumericTypeToString(desc.depth);
    }

    if (desc.dimension == TextureDimension::Texture1DArray ||
        desc.dimension == TextureDimension::Texture2DArray ||
        desc.dimension == TextureDimension::TextureCubeArray ||
        desc.dimension == TextureDimension::Texture2DMSArray) {
        txt += NumericTypeToString(desc.arraySize);
    }

    if (desc.dimension == TextureDimension::Texture1D ||
        desc.dimension == TextureDimension::Texture1DArray ||
        desc.dimension == TextureDimension::Texture2D ||
        desc.dimension == TextureDimension::Texture2DArray ||
        desc.dimension == TextureDimension::TextureCube ||
        desc.dimension == TextureDimension::TextureCubeArray) {
        txt += ", MipLevels =";
        txt += NumericTypeToString(desc.mipLevels);
    }

    if (desc.dimension == TextureDimension::Texture2DMS ||
        desc.dimension == TextureDimension::Texture2DMSArray) {
        txt += ", SampleCount = ";
        txt += NumericTypeToString(desc.sampleCount);
        txt += ", SampleQuality = ";
        txt += NumericTypeToString(desc.sampleQuality);
    }

    if (desc.renderTarget) 
    {
        txt += ", IsRenderTarget";
    }
    if (desc.uav) {
        txt += ", IsUAV";
    }
    if (desc.typeless) 
    {
        txt += ", IsTypeless";
    }

    txt += ")";

    return txt.c_str();
}
core::String Utility::GenerateBufferDebugName(const BufferDesc& desc)
{
    core::String txt = "";
    txt += "Unnamed Buffer (Bytesize_t = ";
    txt += NumericTypeToString(desc.byteSize);

    if (desc.format != FormatType::UNKNOWN) {
        txt += ", Format = ";
        txt += GetFormatInfo(desc.format).name;
    }

    if (desc.structStride > 0) {
        txt += ", StructStride = ";
        txt += NumericTypeToString(desc.structStride);
    }

    if (desc.volatileBuffer) {
        txt += ", IsVolatile, MaxVersions = ";
        txt += NumericTypeToString(desc.maxVersions);
    }

    if (desc.useUavs) 
    {
        txt += ", CanHaveUAVs";
    }
    if (desc.useTypedViews) 
    {
        txt += ", CanHaveTypedViews";
    }

    if (desc.useRawViews) 
    {
        txt += ", CanHaveRawViews";
    }

    if (desc.useVertexBuffer) 
    {
        txt += ", IsVertexBuffer";
    }
    if (desc.useIndexBuffer) 
    {
        txt += ", IsIndexBuffer";
    }
    if (desc.useConstantBuffer) 
    {
        txt += ", IsConstantBuffer";
    }

    if (desc.drawIndirectArgs) 
    {
        txt += ", IsDrawIndirectArgs";
    }

    if (desc.accelStructBuildInput) 
    {
        txt += ", IsAccelStructBuildInput";
    }

    if (desc.accelStructStorage) 
    {
        txt += ", IsAccelStructStorage";
    }

    if (desc.shaderBindingTable) 
    {
        txt += ", IsShaderBindingTable";
    }

    txt += ")";

    return txt.c_str();
}

ResourceType Utility::GetNormalizedResourceType(ResourceType type)
{
    switch (type) {
    case ResourceType::StructuredBuffer_UAV:
    case ResourceType::RawBuffer_UAV:
        return ResourceType::TypedBuffer_UAV;
    case ResourceType::StructuredBuffer_SRV:
    case ResourceType::RawBuffer_SRV:
        return ResourceType::TypedBuffer_SRV;
    default:
        return type;
    }
}

bool Utility::AreResourceTypesCompatible(ResourceType a, ResourceType b)
{
    if (a == b)
    {
        return true;
    }

    a = GetNormalizedResourceType(a);
    b = GetNormalizedResourceType(b);

    if (a == ResourceType::TypedBuffer_SRV && b == ResourceType::Texture_SRV ||
        b == ResourceType::TypedBuffer_SRV && a == ResourceType::Texture_SRV ||
        a == ResourceType::TypedBuffer_SRV && b == ResourceType::RayTracingAccelStruct ||
        a == ResourceType::Texture_SRV && b == ResourceType::RayTracingAccelStruct ||
        b == ResourceType::TypedBuffer_SRV && a == ResourceType::RayTracingAccelStruct ||
        b == ResourceType::Texture_SRV && a == ResourceType::RayTracingAccelStruct)
        return true;

    if (a == ResourceType::TypedBuffer_UAV && b == ResourceType::Texture_UAV ||
        b == ResourceType::TypedBuffer_UAV && a == ResourceType::Texture_UAV)
        return true;

    return false;
}

}
}
