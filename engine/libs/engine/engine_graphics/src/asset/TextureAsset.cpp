#include "engine_graphics/asset/TextureAsset.hpp"
#include "engine_graphics/parser/TextureParser.hpp"

namespace slug
{
namespace engine_graphics
{

SLUG_GRAPHICS_ASSET_REGISTER(TextureAsset)

TextureAsset::TextureAsset(engine_core::AssetHeader header, render::DrawContext& context)
    : GraphicsAssetBase(header, context)
    , m_texture(nullptr)
{
}

bool TextureAsset::Load(void* data, size_t size)
{
    TextureParser::InputParam input = {};
    input.data = data;
    input.size = size;
    input.context = &m_context;

    TextureParser::OutParam out = {};

    if (TextureParser::Parse(input, out))
    {
        m_texture = out.texture;
        return true;
    }

    return false;
}

bool TextureAsset::Resolve(const engine_core::AssetResolver& resolver)
{
    return true;
}

bool TextureAsset::Reload(void* data, size_t size)
{
    m_texture.reset();
    Load(data, size);
    return true;
}

void TextureAsset::Destroy()
{
    m_texture.reset();
}

const render::TexturePtr& TextureAsset::GetTexture()
{
    return m_texture;
}

}
}
