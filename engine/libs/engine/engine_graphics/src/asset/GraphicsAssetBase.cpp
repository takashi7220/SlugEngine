#include "engine_graphics/asset/GraphicsAssetBase.hpp"

namespace slug
{
namespace engine_graphics
{

GraphicsAssetBase::GraphicsAssetBase(const engine_core::AssetHeader& header, render::DrawContext& context)
    : AssetBase(header)
    , m_context(context)
{
}



}
}
