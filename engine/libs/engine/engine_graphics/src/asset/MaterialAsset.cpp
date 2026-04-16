#include "engine_graphics/asset/MaterialAsset.hpp"
#include "engine_graphics/parser/MaterialParser.hpp"
#include "engine_core/asset/AssetResolver.hpp"


namespace slug::engine_graphics
{

SLUG_GRAPHICS_ASSET_REGISTER(MaterialAsset)

MaterialAsset::MaterialAsset(engine_core::AssetHeader header, render::DrawContext& context)
    : GraphicsAssetBase(header, context)
    , m_shaderSlot()
    , m_textureSlots()
    , m_material(nullptr)
{

}

bool MaterialAsset::Load(void* data, size_t size)
{
    MaterialParser::InputParam input = {};
    input.data = data;
    input.size = size;
    input.context = &m_context;

    MaterialParser::OutParam out = {};
    if (MaterialParser::Load(input, out))
    {
        m_material = out.material;
        for (size_t i = 0; i < out.textureSlots.size(); i++)
        {
            auto& textureSlot = m_textureSlots.emplace_back();
            textureSlot.slotName = out.textureSlots.at(i).slotName.c_str();
            textureSlot.textureId = out.textureSlots.at(i).textureId;
        }

        m_shaderSlot.shaderId = out.shaderSlot.shaderId;
        return true;
    }

    return true;
}

bool MaterialAsset::Resolve(const engine_core::AssetResolver& resolver)
{
    if (resolver.HasAsset(m_shaderSlot.shaderId))
    {
        m_shaderSlot.shader = resolver.GetAsset<ShaderAsset>(m_shaderSlot.shaderId);
        m_material->SetShader(m_shaderSlot.shader->GetShader());
    }

    for (auto& slot : m_textureSlots)
    {
        if (resolver.HasAsset(slot.textureId))
        {
            slot.texture = resolver.GetAsset<TextureAsset>(slot.textureId);
            m_material->AddTexture(render::SlotInfo {slot.slotName.c_str()}, slot.texture->GetTexture());
        }
    }
    UpdateMaterial();
    return true;
}

bool MaterialAsset::Reload(void* data, size_t size)
{
    m_material.reset();
    m_textureSlots.clear();
    return Load(data, size);
}

void MaterialAsset::Destroy()
{
    m_material.reset();
    m_textureSlots.clear();
}

render::MaterialPtr MaterialAsset::GetMaterial()
{
    return m_material;
}

void MaterialAsset::UpdateMaterial()
{
    m_material->Reflesh(m_context);

}

}
