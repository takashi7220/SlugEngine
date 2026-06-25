#pragma once

#include "render_core/object/PrimitiveDrawObject.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/container/String.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/container/Vector.hpp"
#include "core/uuid/UUID.hpp"

namespace slug::render
{

using SceneID = core::FixedString<32>;

class Scene : public core::ReferenceObject
{
public:
    Scene();
    virtual ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene& operator=(Scene&&) = delete;

    SLUG_NODISCARD bool RegisterObject(const render_core::DrawObjectPtr& object);
    void UnregisterObject(render_core::DrawObjectID objectID) noexcept;

    SLUG_NODISCARD const render_core::PrimitiveDrawObjectPtr& GetRebuildPrimitiveDrawObject() noexcept;
    SLUG_NODISCARD const render_core::PrimitiveDrawObjectPtr& GetPrimitiveDrawObject() const noexcept;

private:
    void RebuildPrimitiveDrawObject() noexcept;

private:
    render_core::PrimitiveDrawObjectPtr m_primitiveDrawObject = nullptr;
    core::TUnorderedMap<render_core::DrawObjectID, render_core::DrawObjectPtr, core::UUIDHash> m_drawObjects;
    core::TVector<render_core::DrawObjectID> m_drawObjectOrder;
    bool requireRebuild = false;
};

using ScenePtr = core::TReferencePtr<Scene>;

}
