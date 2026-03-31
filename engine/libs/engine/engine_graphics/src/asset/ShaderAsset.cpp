#include "engine_graphics/asset/ShaderAsset.hpp"
#include "engine_graphics/parser/MaterialParser.hpp"
#include "engine_core/asset/AssetResolver.hpp"

namespace slug
{
namespace engine_graphics
{

SLUG_GRAPHICS_ASSET_REGISTER(ShaderAsset)

ShaderAsset::ShaderAsset(engine_core::AssetHeader header, render::DrawContext& context)
    : GraphicsAssetBase(header, context)
    , m_shader(nullptr)
    , m_textureSlots()
{

}

bool ShaderAsset::Load(void* data, size_t size)
{
    ShaderParser::InputParam input = {};
    input.data = data;
    input.size = size;
    input.context = &m_context;

    ShaderParser::OutParam out = {};
    if (ShaderParser::Load(input, out))
    {
        m_shader = out.shader;
        for (size_t i = 0; i < out.textureSlots.size(); i++)
        {
            auto& textureSlot = m_textureSlots.emplace_back();
            textureSlot.slotName = out.textureSlots.at(i).slotName.c_str();
            textureSlot.textureId = out.textureSlots.at(i).textureId;
        }
        return true;
    }
    return false;

}

bool ShaderAsset::Resolve(const engine_core::AssetResolver& resolver)
{
    for (auto& textureSlot : m_textureSlots)
    {
        const TextureAssetPtr& textureAsset = resolver.GetAsset<TextureAsset>(textureSlot.textureId);
        textureSlot.texture = textureAsset;
        m_shader->AddTexture(render::SlotInfo {textureSlot.slotName.c_str()}, textureAsset->GetTexture());
    }
    return true;
}

bool ShaderAsset::Reload(void* data, size_t size)
{
    m_shader.reset();
    m_textureSlots.clear();
    return Load(data, size);
}

void ShaderAsset::Destroy()
{
    m_shader.reset();
    m_textureSlots.clear();
}

const render::ShaderPtr& ShaderAsset::GetShader()
{
    return m_shader;
}

}
}
