#include "rhi/d3d12/resource/Buffer.hpp"
#include "rhi/d3d12/D3D12Utility.hpp"
#include "rhi/common/Resource.hpp"
#include "rhi/common/Utility.hpp"
#include "core/container/String.hpp"
#include "core/debug/Assert.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/Context.hpp"


namespace slug
{
namespace rhi
{
namespace d3d12
{

Buffer::Buffer(const Context& context, DeviceResources& resources, BufferDesc desc) 
    : BufferStateExtension(desc)
    , desc(desc)
    , m_context(context)
    , m_resources(resources)
{
}

Buffer::~Buffer() 
{
    if (m_clearUav != INVALID_DESCRIPTOR_INDEX) 
    {
        m_resources.shaderResourceViewHeap.ReleaseDescriptor(m_clearUav);
        m_clearUav = INVALID_DESCRIPTOR_INDEX;
    }
}

const BufferDesc& Buffer::GetDesc() const
{
    return desc;
}

Object Buffer::GetNativeObject(ObjectType objectType) const
{
    switch (objectType) 
    {
    case object_types::D3D12_Resource:
        return Object(resource);
    case object_types::SharedHandle:
        return Object(sharedHandle);
    default:
        return nullptr;
    }
}

void Buffer::PostCreate()
{
    gpuVirtualAddress = resource->GetGPUVirtualAddress();

    if (!desc.debugName.empty()) 
    {
        core::WString wname = core::StringUtility::GetWstring(desc.debugName.c_str());
        resource->SetName(wname.c_str());
    }
}

DescriptorIndex Buffer::GetClearUAV()
{
    SLUG_ASSERT(desc.useUavs);

    if (m_clearUav != INVALID_DESCRIPTOR_INDEX) 
    {
        return m_clearUav;
    }

    m_clearUav = m_resources.shaderResourceViewHeap.AllocateDescriptor();
    CreateUAV(m_resources.shaderResourceViewHeap.GetCpuHandle(m_clearUav).ptr, FormatType::R32_UINT, EntireBuffer, ResourceType::TypedBuffer_UAV);
    m_resources.shaderResourceViewHeap.CopyToShaderVisibleHeap(m_clearUav);
    return m_clearUav;
}

void Buffer::CreateCBV(size_t descriptor, BufferRange range) const
{
    SLUG_ASSERT(desc.useConstantBuffer);

    range = range.Resolve(desc);
    SLUG_ASSERT(range.byteSize <= UINT_MAX);

    D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc;
    viewDesc.BufferLocation = resource->GetGPUVirtualAddress() + range.byteOffset;
    viewDesc.SizeInBytes = TAlign((uint32_t)range.byteSize, CONSTANT_BUFFER_OFFSET_SIZE_ALINMENT);
    m_context.device->CreateConstantBufferView(&viewDesc, { descriptor });
}

void Buffer::CreateSRV(size_t descriptor, FormatType format, BufferRange range, ResourceType type) const 
{
    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if (format == FormatType::UNKNOWN) 
    {
        format = desc.format;
    }

    range = range.Resolve(desc);

    switch (type) 
    {
    case ResourceType::StructuredBuffer_SRV:
        SLUG_ASSERT(desc.structStride != 0);
        viewDesc.Format = DXGI_FORMAT_UNKNOWN;
        viewDesc.Buffer.FirstElement = range.byteOffset / desc.structStride;
        viewDesc.Buffer.NumElements = (UINT)(range.byteSize / desc.structStride);
        viewDesc.Buffer.StructureByteStride = desc.structStride;
        break;

    case ResourceType::RawBuffer_SRV:
        viewDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        viewDesc.Buffer.FirstElement = range.byteOffset / 4;
        viewDesc.Buffer.NumElements = (UINT)(range.byteSize / 4);
        viewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
        break;

    case ResourceType::TypedBuffer_SRV:
    {
        SLUG_ASSERT(format != FormatType::UNKNOWN);
        const DxgiFormatMapping& mapping = D3D12Utility::GetDxgiFormatMapping(format);
        const FormatInfo& formatInfo = GetFormatInfo(format);

        viewDesc.Format = mapping.srvFormat;
        viewDesc.Buffer.FirstElement = range.byteOffset / formatInfo.bytesPerBlock;
        viewDesc.Buffer.NumElements = (UINT)(range.byteSize / formatInfo.bytesPerBlock);
        break;
    }

    default:
        SLUG_LOG(RHILogCategory, core::LogType::Error, "InvalidEnum ResourceType");
        return;
    }

    m_context.device->CreateShaderResourceView(resource, &viewDesc, { descriptor });
}

void Buffer::CreateUAV(size_t descriptor, FormatType format, BufferRange range, ResourceType type) const 
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc = {};

    viewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

    if (format == FormatType::UNKNOWN) 
    {
        format = desc.format;
    }

    range = range.Resolve(desc);

    switch (type)
    {
    case ResourceType::StructuredBuffer_UAV:
        SLUG_ASSERT(desc.structStride != 0);
        viewDesc.Format = DXGI_FORMAT_UNKNOWN;
        viewDesc.Buffer.FirstElement = range.byteOffset / desc.structStride;
        viewDesc.Buffer.NumElements = (UINT)(range.byteSize / desc.structStride);
        viewDesc.Buffer.StructureByteStride = desc.structStride;
        break;

    case ResourceType::RawBuffer_UAV:
        viewDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        viewDesc.Buffer.FirstElement = range.byteOffset / 4;
        viewDesc.Buffer.NumElements = (UINT)(range.byteSize / 4);
        viewDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
        break;

    case ResourceType::TypedBuffer_UAV:
    {
        SLUG_ASSERT(format != FormatType::UNKNOWN);
        const DxgiFormatMapping& mapping = D3D12Utility::GetDxgiFormatMapping(format);
        const FormatInfo& formatInfo = GetFormatInfo(format);

        viewDesc.Format = mapping.srvFormat;
        viewDesc.Buffer.FirstElement = range.byteOffset / formatInfo.bytesPerBlock;
        viewDesc.Buffer.NumElements = (UINT)(range.byteSize / formatInfo.bytesPerBlock);
        break;
    }

    default:
        SLUG_LOG(RHILogCategory, core::LogType::Error, "InvalidEnum ResourceType");
        return;
    }

    m_context.device->CreateUnorderedAccessView(resource, nullptr, &viewDesc, { descriptor });
}

void Buffer::CreateNullSRV(size_t descriptor, FormatType format, const Context& context) 
{
    const DxgiFormatMapping& mapping = D3D12Utility::GetDxgiFormatMapping(format == FormatType::UNKNOWN ? FormatType::R32_UINT : format);

    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    viewDesc.Format = mapping.srvFormat;
    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    context.device->CreateShaderResourceView(nullptr, &viewDesc, { descriptor });
}

void Buffer::CreateNullUAV(size_t descriptor, FormatType format, const Context& context) 
{
    const DxgiFormatMapping& mapping = D3D12Utility::GetDxgiFormatMapping(format == FormatType::UNKNOWN ? FormatType::R32_UINT : format);

    D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc = {};
    viewDesc.Format = mapping.srvFormat;
    viewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    context.device->CreateUnorderedAccessView(nullptr, nullptr, &viewDesc, { descriptor });
}

}
}
}
