#include "sgal/structure/octree/Octree.hpp"

namespace slug
{
namespace sgal
{

void OctreeBase::Node::Subdivide()
{
    math::Vector3 center = boundingBox.Center();
    math::Vector3 min = boundingBox.min;
    math::Vector3 max = boundingBox.max;

    for (int i = 0; i < NODE_CHILD_NUM; i++)
    {
        math::Vector3 newMin = math::Vector3(
                (i & 1) ? center.x : min.x,
                (i & 2) ? center.y : min.y,
                (i & 4) ? center.z : min.z
        );
        math::Vector3 newMax = math::Vector3(
            (i & 1) ? max.x : center.x,
            (i & 2) ? max.y : center.y,
            (i & 4) ? max.z : center.z
        );
        math::AABB childBox;
        childBox.min = newMin;
        childBox.max = newMax;
        children[i] = core::MakeUnique<OctreeBase::Node>();
        children[i]->boundingBox = childBox;
        children[i]->depth = depth + 1;
    }
}

core::TUniquePtr<OctreeBase::Node>& OctreeBase::GetTopNode()
{
    return m_topNode;
}
}
}
