#include "render/scene/Scene.hpp"

namespace slug::render
{

Scene::Scene()
    : m_primitiveDrawObject(core::MakeReference<render_core::PrimitiveDrawObject>())
{
}

bool Scene::RegisterObject(const render_core::DrawObjectPtr& object)
{
    if (object == nullptr)
    {
        return false;
    }

    const render_core::DrawObjectID objectID = object->GetDrawObjectID();
    if (m_drawObjects.find(objectID) == m_drawObjects.end())
    {
        m_drawObjectOrder.push_back(objectID);
    }

    m_drawObjects[objectID] = object;
    requireRebuild = true;
    return true;
}

void Scene::UnregisterObject(render_core::DrawObjectID objectID) noexcept
{
    m_drawObjects.erase(objectID);
    auto itr = m_drawObjectOrder.begin();
    while (itr != m_drawObjectOrder.end())
    {
        if (*itr == objectID)
        {
            itr = m_drawObjectOrder.erase(itr);
        }
        else
        {
            itr++;
        }
    }

    requireRebuild = true;
}

const render_core::PrimitiveDrawObjectPtr& Scene::GetRebuildPrimitiveDrawObject() noexcept
{
    if (requireRebuild)
    {
        RebuildPrimitiveDrawObject();
    }
    return GetPrimitiveDrawObject();
}

const render_core::PrimitiveDrawObjectPtr& Scene::GetPrimitiveDrawObject() const noexcept
{
    return m_primitiveDrawObject;
}

void Scene::RebuildPrimitiveDrawObject() noexcept
{
    if (m_primitiveDrawObject == nullptr)
    {
        m_primitiveDrawObject = core::MakeReference<render_core::PrimitiveDrawObject>();
    }

    m_primitiveDrawObject->drawObjects = core::TQueue<render_core::DrawObjectPtr>();
    for (const auto& objectID : m_drawObjectOrder)
    {
        const auto object = m_drawObjects.find(objectID);
        if (object != m_drawObjects.end() && object->second != nullptr)
        {
            m_primitiveDrawObject->drawObjects.push(object->second);
        }
    }
    requireRebuild = false;
}

}
