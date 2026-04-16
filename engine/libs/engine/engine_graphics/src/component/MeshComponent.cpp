#include "engine_graphics/component/MeshComponent.hpp"

namespace slug::engine_graphics
{

SLUG_COMPONENT_REGISTER(MeshComponent)

MeshComponent::MeshComponent()
{

}

MaterialAssetPtr MeshComponent::GetMaterialAsset(core::StringView slotName)
{
    MaterialAssetPtr material = nullptr;
    bool isFound = false;
    for (auto& materialSlot : m_materialSlots)
    {
        if (materialSlot.slotName == slotName)
        {
            if (materialSlot.material != nullptr)
            {
                material = materialSlot.material;
                isFound = true;
                break;
            }
        }
    }

    if (!isFound)
    {
        material = m_model->GetMaterial(slotName);
    }

    return material;
}

void MeshComponent::GatherDrawObject(const render::PrimitiveDrawSettings& settings, render::PrimitiveDrawObjectPtr& primitiveDrawObject)
{
    const render::ModelPtr& model = m_model->GetModel();
    if (model == nullptr)
    {
        return;
    }

    render::SubModelPtr subModel = model->GetSubModel(GetWorldMatrix(), settings.view);
    if (subModel == nullptr)
    {
        return;
    }

    UpdateShaderParameter(settings);
    render::DrawObject::Settings drawSettings = GetSettings();

    for(auto& mesh : subModel->meshes)
    {
        if (mesh != nullptr)
        {
            MaterialAssetPtr materialAsset = GetMaterialAsset(mesh->GetMaterialSlotName());
            if (materialAsset != nullptr)
            {
                render::MaterialPtr material = materialAsset->GetMaterial();
                render::DrawObjectPtr drawObject = render::DrawObject::Create(drawSettings, m_shaderParameter, mesh, material);
                primitiveDrawObject->drawObjects.push(drawObject);
            }
        }
    }
}

math::AABB MeshComponent::GetBoundingBox(bool applyWorldMatrix)
{
    const render::ModelPtr& model = m_model->GetModel();
    if (model == nullptr)
    {
        return math::AABB();
    }

    render::SubModelPtr subModel = model->mainModel;
    if (subModel == nullptr)
    {
        return math::AABB();
    }

    if (applyWorldMatrix)
    {
        return subModel->boundingBox * this->GetWorldMatrix();
    }
    
    return subModel->boundingBox;
}

}
