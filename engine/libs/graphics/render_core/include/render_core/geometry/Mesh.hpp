#pragma once

#include "render_core/material/Material.hpp"
#include "render_core/geometry/VertexAttribute.hpp"
#include "algorithm/primitive/Aabb.hpp"


namespace slug::render_core
{

class RenderService;
class Mesh;
using MeshPtr = core::TReferencePtr<Mesh>;
using BoundingBox = algorithm::Aabb;

class Mesh : public core::ReferenceObject
{
public:
    struct IndexAccessor
    {
        core::FixedString<64> attributeName;
        render_core::FormatType format = render_core::FormatType::R32_SINT;
        size_t size;
        size_t count;
        size_t bufferIndex;
    };

    struct VertexAttributeAccessor
    {
        render_core::FormatType format = render_core::FormatType::RGBA32_FLOAT;
        size_t size;
        size_t offset;
        core::FixedString<64> attributeName;
        uint32_t attributeIndex;
    };

    struct VertexAccessor
    {
        core::TVector<VertexAttributeAccessor> attributeAccessors;
        uint32_t bufferIndex;
        size_t count;
    };

    struct Buffer
    {
        core::FixedString<256> name;
        size_t size;
        void* data;
    };

    struct SetupParameter
    {
        VertexAccessor vertexAccessor = {};
        IndexAccessor indexAccessor = {};
        core::TVector<Buffer> buffers = {};
        core::StringView materialSlotName = {};
        bool write = true;
    };

public:

    static MeshPtr Create(RenderService& service,const SetupParameter& param);

    explicit Mesh();
    virtual ~Mesh();
    virtual size_t GetTriangleCount() const;
    virtual size_t GetVertexCount() const;
    virtual size_t GetIndexCount() const;

    core::StringView GetMaterialSlotName();
    void SetMaterial(const MaterialPtr& material);
    const MaterialPtr& GetMaterial() const;

    const GeometroyAttributePtr& GetGeometry() const;

    void SetBoundingBox(const BoundingBox& boundingBox);
    const BoundingBox& GetBoundingBox() const;

    void SetVertexBuffer(const rhi::BufferPtr& buffer);
    const rhi::BufferPtr& GetVertexBuffer() const;
    void WriteVertexBuffer(RenderService& service, const VertexAccessor& accessor, const Buffer& buffers);

    void SetIndexBuffer(const rhi::BufferPtr& buffer);
    const rhi::BufferPtr& GetIndexBuffer() const;
    void WriteIndexBuffer(RenderService& service, const IndexAccessor& accessor, const Buffer& buffers);
private:
    static rhi::BufferPtr CreateVertexBuffer(RenderService& service, const VertexAccessor& accessor, const core::TVector<Buffer>& buffers);
    static rhi::BufferPtr CreateIndexBuffer(RenderService& service, const IndexAccessor& accessor, const core::TVector<Buffer>& buffers);
    static rhi::BufferPtr CreateBuffer(RenderService& service, size_t size, bool useVertexBuffer,core::StringView name);

    void SetupGeometryAttribute(const SetupParameter& param);
private:
    GeometroyAttributePtr m_geometryAttribute = {};
    BoundingBox m_boundingBox = {};
    MaterialPtr m_material = nullptr;
    core::FixedString<128> m_materialSlotName = {};
};

}
