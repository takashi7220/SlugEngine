#include "engine_graphics/asset/ModelAsset.hpp"
#include "engine_graphics/parser/ModelParser.hpp"
#include "engine_core/asset/AssetResolver.hpp"

namespace slug::engine_graphics
{

SLUG_GRAPHICS_ASSET_REGISTER(ModelAsset)

ModelAsset::ModelAsset(engine_core::AssetHeader header, render::DrawContext& context)
    : GraphicsAssetBase(header, context)
    , m_materialSlots()
    , m_model(nullptr)
{
}

bool ModelAsset::Load(void* data, size_t size)
{
    ModelParser::InputParam input = {};
    input.data = data;
    input.size = size;
    input.context = &m_context;

    ModelParser::OutParam out = {};
    if (ModelParser::Parse(input, out))
    {
        m_model = out.model;

        for (size_t i = 0; i < out.materialSlots.size(); i++)
        {
            auto& dstMaterialSlot = m_materialSlots.emplace_back();
            auto& srcMaterialSlot = out.materialSlots.at(i);

            dstMaterialSlot.slotName = srcMaterialSlot.slotName;
            dstMaterialSlot.materialId = srcMaterialSlot.materialId;
        }
        return true;
    }

    return false;
}

bool ModelAsset::Resolve(const engine_core::AssetResolver& resolver)
{
    for (auto& materialSlot : m_materialSlots)
    {
        if (resolver.HasAsset(materialSlot.materialId))
        {
            materialSlot.material = resolver.GetAsset<MaterialAsset>(materialSlot.materialId);
        }
    }
    UpdateMaterial();
    return true;
}

bool ModelAsset::Reload(void* data, size_t size)
{
    m_model.reset();
    m_materialSlots.clear();
    return Load(data, size);
}

void ModelAsset::Destroy()
{
    m_model.reset();
    m_materialSlots.clear();
}

const render::ModelPtr& ModelAsset::GetModel() const
{
    return m_model;
}

void ModelAsset::GetMaterialSlotNames(core::TVector<core::FixedString<128>>& slotNames, bool withLod) const
{
    for (auto& materialSlot : m_materialSlots)
    {
        slotNames.push_back(materialSlot.slotName);
    }
}

const MaterialAssetPtr ModelAsset::GetMaterial(core::StringView slotName) const
{
    for (auto& materialSlot : m_materialSlots)
    {
        if (materialSlot.slotName == slotName)
        {
            return materialSlot.material;
        }
    }
    return nullptr;
}


void ModelAsset::SetMaterial(core::StringView slotName, const MaterialAssetPtr& material)
{
    for (auto& materialSlot : m_materialSlots)
    {
        if (materialSlot.slotName == slotName)
        {
            materialSlot.material = material;
            materialSlot.materialId = material->GetID();
        }
    }

    UpdateMaterial();
}

void ModelAsset::UpdateMaterial()
{
    core::TVector<MaterialSlot> slots = {};

    for (auto& materialSlot : m_materialSlots)
    {
        slots.push_back(materialSlot);
    }

    for (auto& materialSlot : slots)
    {
        for (auto& mesh : m_model->mainModel->meshes)
        {
            if (materialSlot.slotName == mesh->GetMaterialSlotName())
            {
                mesh->SetMaterial(materialSlot.material->GetMaterial());
            }
        }

        for (auto& subModel : m_model->lodModel->subModels)
        {
            for (auto& mesh : subModel->meshes)
            {
                if(materialSlot.slotName == mesh->GetMaterialSlotName())
                {
                    mesh->SetMaterial(materialSlot.material->GetMaterial());
                }
            }
        }
    }
}

}
