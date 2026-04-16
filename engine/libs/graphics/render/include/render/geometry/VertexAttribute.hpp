#pragma once

#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "rhi/Rhi.hpp"

#define RENDER_VERTEX_ATTRIBUTE_COUNT 

namespace slug::render
{

struct VertexAttributeItem
{
    core::FixedString<256> attributeName = {};
    uint32_t semanticIndex = 0;
    rhi::FormatType formatType = rhi::FormatType::RGBA16_FLOAT;
    rhi::BufferRange bufferRange = {};
};

struct VertexAttribute
{
    rhi::BufferPtr buffer = nullptr;
    core::TVector<VertexAttributeItem> attributes = {};
    size_t count = 0;
};

struct IndexAttribute
{
    rhi::BufferPtr buffer = nullptr;
    rhi::FormatType formatType = rhi::FormatType::R32_UINT;
    size_t count = 0;
};

struct GeometroyAttribute : core::ReferenceObject
{
    VertexAttribute vertexAttribute;
    IndexAttribute indexAttribute;
};

using GeometroyAttributePtr = core::TReferencePtr<GeometroyAttribute>;
}
