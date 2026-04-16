#include "sgal/structure/octree/OctreeFlat.hpp"
#include "core/container/Queue.hpp"

namespace slug::sgal
{


struct TraverseItem
{
    math::AABB boundingBox;
    uint32_t index = 0;
    uint32_t depth = 0;

    TraverseItem()
        : index(0)
        , depth(0)
    {
    }

    TraverseItem(const math::AABB& _boundingBox, uint32_t _index = 0, uint32_t _depth = 0)
        : boundingBox(_boundingBox)
        , index(_index)
        , depth(_depth)
    {
    }
};

OctreeFlatBase::OctreeFlatBase()
{

}

OctreeFlatBase::~OctreeFlatBase()
{

}

const OctreeFlatBase::Header& OctreeFlatBase::GetHeader()
{
    return m_header;
}

void OctreeFlatBase::SetHeader(const Header& header)
{
    m_header = header;
}

void OctreeFlatBase::AddNode(const Node& node)
{
    m_nodes.push_back(node);
}

void OctreeFlatBase::Traverse(TraverseCallback callback) const
{
    TraverseInternal(nullptr, callback);
}


void OctreeFlatBase::TraverseHit(HitInterface* hit, TraverseCallback callback) const
{
    TraverseInternal(hit, callback);
}

void OctreeFlatBase::TraverseInternal(HitInterface* hit, TraverseCallback callback) const
{
    core::TQueue<TraverseItem> queue;
    queue.push(TraverseItem(m_header.boudingBox));
    while (!queue.empty())
    {
        TraverseItem item = queue.front();
        queue.pop();

        if (item.index >= m_nodes.size() || item.depth > m_header.maxDepth)
        {
            continue;
        }

        const Node& node = m_nodes[item.index];

        bool enableCallback = false;
        if (hit != nullptr)
        {
            if (!hit->Intersect(item.boundingBox))
            {
                continue;
            }
            else if (node.leaf)
            {
                enableCallback = true;
            }
        }
        else
        {
            enableCallback = true;
        }

        if (enableCallback)
        {
            CallbackParameter parameter;
            parameter.boundingBox = item.boundingBox;
            parameter.depth = item.depth;
            parameter.leaf = node.leaf;
            parameter.dataIndex = node.dataIndex;
            callback(parameter);
        }

        if (!node.leaf)
        {
            uint32_t childBase = node.childBlockOffset;
            math::Vector3 center = item.boundingBox.Center();
            uint32_t childOffset = 0;
            for (int32_t i = 0; i < NODE_CHILD_NUM; ++i)
            {
                math::AABB childBoundingBox;
                childBoundingBox.min = math::Vector3(
                    (i & 1) ? center.x : item.boundingBox.min.x,
                    (i & 2) ? center.y : item.boundingBox.min.y,
                    (i & 4) ? center.z : item.boundingBox.min.z
                );

                childBoundingBox.max = math::Vector3(
                    (i & 1) ? item.boundingBox.max.x : center.x,
                    (i & 2) ? item.boundingBox.max.y : center.y,
                    (i & 4) ? item.boundingBox.max.z : center.z
                );

                if (node.childMask & (1 << i))
                {
                    uint32_t childIndex = childBase + childOffset;
                    queue.push(TraverseItem(childBoundingBox, childIndex, item.depth + 1));
                    ++childOffset;
                }
            }
        }
    }
}

}
