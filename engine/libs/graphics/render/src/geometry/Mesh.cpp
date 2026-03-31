#include "render/geometry/Mesh.hpp"
#include "render/context/DrawContext.hpp"

namespace slug
{
namespace render
{

Mesh* Mesh::Create(DrawContext& context, const SetupParameter& param)
{
    MeshPtr mesh(new Mesh);
    rhi::BufferPtr vertexBuffer = CreateVertexBuffer(context, param.vertexAccessor, param.buffers);
    rhi::BufferPtr indexBuffer = CreateIndexBuffer(context, param.indexAccessor, param.buffers);
    mesh->SetVertexBuffer(vertexBuffer);
    mesh->SetIndexBuffer(indexBuffer);
    mesh->SetupGeometryAttribute(param);
    mesh->m_materialSlotName = param.materialSlotName;
    if (param.write)
    {
        mesh->WriteVertexBuffer(context, param.vertexAccessor, param.buffers.at(param.vertexAccessor.bufferIndex));
        mesh->WriteIndexBuffer(context, param.indexAccessor, param.buffers.at(param.indexAccessor.bufferIndex));
    }
    return mesh;
}

Mesh::Mesh()
    : m_geometryAttribute(new GeometroyAttribute)
{
}

Mesh::~Mesh()
{
}

size_t Mesh::GetTriangleCount() const
{
    return GetIndexCount() / 3;
}

size_t Mesh::GetVertexCount() const
{
    return m_geometryAttribute->vertexAttribute.count;
}

size_t Mesh::GetIndexCount() const
{
    return m_geometryAttribute->indexAttribute.count;
}

core::StringView Mesh::GetMaterialSlotName()
{
    return m_materialSlotName.c_str();
}

void Mesh::SetMaterial(const MaterialPtr& material)
{
    m_material = material;
}

const MaterialPtr& Mesh::GetMaterial() const
{
    return m_material;
}

const GeometroyAttributePtr& Mesh::GetGeometry() const
{
    return m_geometryAttribute;
}

void Mesh::SetBoundingBox(const BoundingBox& boundingBox)
{
    m_boundingBox = boundingBox;
}

const BoundingBox& Mesh::GetBoundingBox() const
{
    return m_boundingBox;
}

void Mesh::SetVertexBuffer(const rhi::BufferPtr& buffer)
{
    m_geometryAttribute->vertexAttribute.buffer = buffer;
}

const rhi::BufferPtr& Mesh::GetVertexBuffer() const
{
    return m_geometryAttribute->vertexAttribute.buffer;
}

void Mesh::WriteVertexBuffer(DrawContext& context, const VertexAccessor& accessor, const Buffer& buffer)
{
    if (buffer.data != nullptr)
    {
        context.GetCreateResourceCommand()->BeginTrackingBufferState(m_geometryAttribute->vertexAttribute.buffer, rhi::ResourceStates::CopyDest);
        context.GetCreateResourceCommand()->WriteBuffer(m_geometryAttribute->vertexAttribute.buffer, buffer.data, buffer.size);
        context.GetCreateResourceCommand()->SetPermanentBufferState(m_geometryAttribute->vertexAttribute.buffer, rhi::ResourceStates::VertexBuffer);
    }

}

void Mesh::SetIndexBuffer(const rhi::BufferPtr& buffer)
{
    m_geometryAttribute->indexAttribute.buffer = buffer;
}

const rhi::BufferPtr& Mesh::GetIndexBuffer() const
{
    return m_geometryAttribute->indexAttribute.buffer;
}



void Mesh::WriteIndexBuffer(DrawContext& context, const IndexAccessor& accessor,const Buffer& buffer)
{
    if (buffer.data != nullptr)
    {
        context.GetCreateResourceCommand()->BeginTrackingBufferState(m_geometryAttribute->indexAttribute.buffer, rhi::ResourceStates::CopyDest);
        context.GetCreateResourceCommand()->WriteBuffer(m_geometryAttribute->indexAttribute.buffer, buffer.data, buffer.size);
        context.GetCreateResourceCommand()->SetPermanentBufferState(m_geometryAttribute->indexAttribute.buffer, rhi::ResourceStates::IndexBuffer);
    }
}

rhi::BufferPtr Mesh::CreateVertexBuffer(DrawContext& context, const VertexAccessor& accessor, const core::TVector<Buffer>& buffers)
{
    const Buffer& buffer = buffers.at(accessor.bufferIndex);
    return CreateBuffer(context, buffer.size, true, buffer.name.c_str());
}

rhi::BufferPtr Mesh::CreateIndexBuffer(DrawContext& context, const IndexAccessor& accessor, const core::TVector<Buffer>& buffers)
{
    const Buffer& buffer = buffers.at(accessor.bufferIndex);
    return CreateBuffer(context, buffer.size, false, buffer.name.c_str());
}

rhi::BufferPtr Mesh::CreateBuffer(DrawContext& context, size_t size, bool useVertexBuffer, core::StringView name)
{
    rhi::BufferDesc desc;
    desc.byteSize = size;
    desc.useVertexBuffer = useVertexBuffer;
    desc.useIndexBuffer = !useVertexBuffer;
    desc.debugName = name;
    desc.initialState = rhi::ResourceStates::CopyDest;

    uint64_t key = core::HashUtility::ConvertStringToHash64(name);
    auto service = context.GetResourceService();

    rhi::BufferPtr buffer = nullptr;
    if (service->GetBufferPool()->Get(key, buffer))
    {
        return buffer;
    }

    if (buffer == nullptr)
    {
        buffer = context.GetDevice()->CreateBuffer(desc);
    }
    return buffer;
}

void Mesh::SetupGeometryAttribute(const SetupParameter& param)
{
    m_geometryAttribute->vertexAttribute.count = param.vertexAccessor.count;
    for (auto& accessor : param.vertexAccessor.attributeAccessors)
    {
        VertexAttributeItem& attributeItem = m_geometryAttribute->vertexAttribute.attributes.emplace_back();
        attributeItem.attributeName = accessor.attributeName.c_str();
        attributeItem.formatType = static_cast<rhi::FormatType>(accessor.format);
        attributeItem.semanticIndex = accessor.attributeIndex;
        attributeItem.bufferRange.byteSize = accessor.size;
        attributeItem.bufferRange.byteOffset = accessor.offset;
    }

    m_geometryAttribute->indexAttribute.count = param.indexAccessor.count;
    m_geometryAttribute->indexAttribute.formatType = static_cast<rhi::FormatType>(param.indexAccessor.format);
}

}
}
