#include "engine_graphics/asset/GraphicsAssetBase.hpp"

namespace slug::engine_graphics
{

GraphicsAssetBase::GraphicsAssetBase(const engine_core::AssetHeader& header, render_core::RenderService& service)
    : AssetBase(header)
    , m_context(context)
{
}



}
