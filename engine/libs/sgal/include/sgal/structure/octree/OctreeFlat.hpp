#pragma once

#include "math/AABB.hpp"
#include "core/container/Vector.hpp"
#include "core/Function.hpp"
#include "sgal/structure/HitInterface.hpp"

namespace slug
{
namespace sgal
{

class OctreeFlatBase
{
public:
    constexpr static uint32_t NODE_CHILD_NUM = 8;

    struct Header
    {
        math::AABB boudingBox = {};
        math::Vector3 nodeSize = {};
        int32_t nodeNum = 0;
        uint32_t maxDepth = 0;
    };

    struct Node
    {
        int32_t dataIndex = 0;
        uint32_t childBlockOffset = 0;
        uint8_t childMask = 0;
        bool leaf = false;
    };

    struct CallbackParameter
    {
        math::AABB boundingBox = {};
        int32_t dataIndex = 0;
        int32_t depth = 0;
        bool leaf = false;
    };

    using TraverseCallback = core::TFunctionObject<void(const CallbackParameter& parameter)>;
    using HitCallback = core::TFunctionObject<void(const CallbackParameter& parameter)>;

public:
    OctreeFlatBase();
    virtual ~OctreeFlatBase();

    const Header& GetHeader();
    void SetHeader(const Header& header);

    void AddNode(const Node& node);

    void Traverse(TraverseCallback callback) const;
    void TraverseHit(HitInterface* hit, TraverseCallback callback) const;

private:
    void TraverseInternal(HitInterface* hit, TraverseCallback callback) const;

protected:
    Header m_header;
    core::TVector<Node> m_nodes;
};

template<class T>
class OctreeFlat : public OctreeFlatBase
{
public:

public:
    OctreeFlat()
    {
    }

    const T* GetData(int32_t index)
    {
        if (m_datas.size() > index)
        {
            return &m_datas.at(index);
        }
        return nullptr;
    }

    void AddData(const T& data)
    {
        m_datas.push_back(data);
    }
private:
    core::TVector<T> m_datas;

};

}
}
