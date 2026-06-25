#pragma once

#include "sgal/geometry/Mesh.hpp"

namespace slug::algorithm
{
class GeometryUtility
{
public:
    static void Triangulate(Mesh& mesh);
    static void Triangulate();
    static void SplitPerMaterial();
    static void MergePerMaterial();
    static void ComputeNormals(Mesh& mesh);
    static void ComputeNormals();
    static void ComputeTangents(Mesh& mesh);
    static void ComputeTangents();
    static void ComputeBoundingBox(Mesh& mesh);
    static void ComputeBoundingBox();
}

}
