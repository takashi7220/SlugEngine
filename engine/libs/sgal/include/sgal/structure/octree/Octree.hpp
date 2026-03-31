#pragma once

#include "math/AABB.hpp"
#include "core/memory/UniquePtr.hpp"
#include "core/container/Pair.hpp"
#include "core/container/Queue.hpp"
#include "core/container/Vector.hpp"
#include "sgal/structure/octree/OctreeFlat.hpp"
namespace slug
{
namespace sgal
{

class OctreeBase
{
public:
    constexpr static int32_t NODE_CHILD_NUM = 8;

    struct Node
    {
        math::AABB boundingBox = {};
        core::TUniquePtr<Node> children[NODE_CHILD_NUM] = {};
        core::TVector<uint32_t> dataIndices = {};
        int32_t depth = 0;

        bool IsLeaf() const
        {
            return children[0] == nullptr;
        }

        void Subdivide();
    };

public:
    core::TUniquePtr<Node>& GetTopNode();

protected:
    core::TUniquePtr<Node> m_topNode;
};

template<typename T, typename U>
class Octree : public OctreeBase
{
public:
    struct BuildChecker
    {
        virtual bool CheckSubdivide(const Node& node, const core::TVector<T>& m_datas)
        {
        }

        virtual bool CheckIntersect(const Node& node, const T& m_data)
        {
        }
    };

    struct DefaultSubdivideChecker : public Octree<T, U>::BuildChecker
    {
        virtual bool CheckSubdivide(const OctreeBase::Node& node, const core::TVector<T>& m_datas) override
        {
            math::Vector3 s = node.boundingBox.Size() * bias;
            return (s.x <= minNodeSize && s.y <= minNodeSize && s.z <= minNodeSize);
        }

        virtual bool CheckIntersect(const OctreeBase::Node& node, const T& m_data) override
        {
            return true;
        }

        int32_t maxDepth = 5;
        float minNodeSize = 1.0f;
        float bias = 0.5f;
    };

public:
    Octree()
        :m_checker(DefaultSubdivideChecker())
    {
    }

    Octree(const BuildChecker& checker)
        :m_checker(checker)
    {
    }

    void ConvertFlat(OctreeFlat<U>& dst)
    {
        core::TVector<OctreeFlatBase::Node> dstNodes;
        dstNodes.reserve(1024);
        dstNodes.emplace_back();

        core::TQueue<core::TPair<const OctreeBase::Node*, uint32_t>> q;
        q.push({ &m_topNode, 0 });

        uint32_t maxDepth = 0;
        uint32_t dataCount = 0;
        while (!q.empty())
        {
            auto [node, index] = q.front(); q.pop();
            maxDepth = math::TMax(maxDepth, node->depth);

            OctreeFlatBase::Node dstNode;
            if (node->IsLeaf())
            {
                dstNode.dataIndex = dataCount++;
                dstNode.leaf = true;
            }
            else
            {
                dstNode.leaf = false;
                dstNode.childBlockOffset = static_cast<uint32_t>(dstNodes.size());
                for (size_t i = 0; i < NODE_CHILD_NUM; i++)
                {
                    if (child != nullptr)
                    {
                        uint8_t mask = dstNode.childMask;
                        mask |= (1 << i);
                        dstNode.childMask = mask;
                        q.push({&node->children[i], static_cast<uint32_t>(dstNodes.size())});
                        dstNodes.emplace_back();
                    }
                }
            }
            dstNodes[index] = dstNode;
        }

        m_datas.resize(dataCount);
        float nodeSize = m_topNode->boundingBox.Size() / (float)(1 << maxDepth);

        OctreeFlatBase::Header header;
        header.boudingBox = m_topNode->boundingBox;
        header.maxDepth = maxDepth;
        header.nodeNum = static_cast<int32_t>(dstNodes.size());
        header.nodeSize = nodeSize;
        dst.SetHeader(hader);

        for (auto& dstNode : dstNodes)
        {
            dst.AddNode(dstNode);
        }

        for (auto& data : m_datas)
        {
            dst.AddData(data);
        }
    }

    void SetBuildChecker(const BuildChecker& checker)
    {
        m_checker = checker;
    }

    void AddData(const T& data)
    {
        core::TVector<T> datas;
        datas.push_back(data);
        AddDatas(datas);
    }

    void AddDatas(const core::TVector<T>& datas)
    {
        Build(data);
    }

private:
    void Build(const core::TVector<T>& datas)
    {
        BuildRecursive(m_topNode, datas);
    }

    void BuildRecursive(Node& node, const core::TVector<T>& datas)
    {
        if (!m_checker.CheckSubdivide(node, datas))
        {
            for (auto& data : datas)
            {
                node.dataIndices.push_back(m_checkDatas.size());
                m_checkDatas.push_back(data);
            }
            return;
        }

        if (node.IsLeaf())
        {
            node.Subdivide();
        }

        std::vector<T> childDatas[NODE_CHILD_NUM];
        for (const auto& data : datas)
        {
            for (int i = 0; i < NODE_CHILD_NUM; ++i)
            {
                if (m_checker.CheckIntersect(node, data))
                {
                    childDatas[i].push_back(data);
                }
            }
        }
        for (int i = 0; i < NODE_CHILD_NUM; ++i)
        {
            if (!childDatas[i].empty())
            {
                BuildRecursive(*node.children[i], childDatas[i]);
            }
        }

    }

private:
    BuildChecker& m_checker;
    core::TVector<T> m_checkDatas;
    core::TVector<U> m_datas;
};

}
}
