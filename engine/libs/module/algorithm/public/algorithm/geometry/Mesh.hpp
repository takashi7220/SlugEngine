#pragma once

#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "core/container/String.hpp"
#include "sgal/geometry/Attribute.hpp"

namespace slug::algorithm
{
struct VertexInfo
{
public:
    template<typename T>
    struct LayerElement
    {
        int32_t index = -1;
        core::FixedString<64> name = {};
        T value;
    };

    struct BoneElement
    {
        int32_t index = -1;
        core::FixedString<64> name = {};
        float weight;
    };

public:
    math::Vector3 normal;
    core::TVector<LayerElement<math::Vector2>> uvs;
    core::TVector<LayerElement<math::Vector3>> tangents;
    core::TVector<LayerElement<math::Vector3>> binormals;
    core::TVector<BoneElement> bones;
    core::TVector<LayerElement<math::Vector4>> colors;
};

struct ControlPoint
{
    math::Vector3 position;
    core::TVector<VertexInfo> vertexInfos;
};

struct Polygon
{
public:
    struct VertexRef
    {
        int32_t controlPointIndex = -1;
        int32_t vertexInfoIndex = -1;
    };

    struct MaterialInfo
    {
        int32_t index = -1;
        core::FixedString<64> name;
    };

public:
    core::TVector<VertexRef> vertices;
    MaterialInfo materialInfo;
};

struct Mesh : Attribute
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(Mesh, Attribute)
    core::TVector<ControlPoint> controlPoints;
    core::TVector<Polygon> polygons;
    static Mesh* Cast(Attribute* attribute)
    {
        return core::SObjectUtility::CheckedCast<Mesh>(attribute);
    }
};



}
