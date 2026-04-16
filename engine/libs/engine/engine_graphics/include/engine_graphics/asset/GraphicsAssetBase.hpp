#pragma once

#include "engine_core/asset/AssetBase.hpp"
#include "engine_graphics/asset/GraphicsAssetLoader.hpp"
#include "render/context/DrawContext.hpp"

#define SLUG_GRAPHICS_ASSET_REGISTER(type)                                                                                                 \
namespace                                                                                                                                  \
{                                                                                                                                          \
    struct AutoRegisterGenerator_##type                                                                                                      \
    {                                                                                                                                      \
        AutoRegisterGenerator_##type()                                                                                                       \
        {                                                                                                                                  \
            slug::engine_graphics::GraphicsAssetUtility::RegisterGenerator(&type::GetStaticAssetType, &type::Create);                        \
        }                                                                                                                                  \
    } autoRegisterGenerator_##type;                                                                                                          \
}                                                                                                                                          \

#define SLUG_GRAPHICS_ASSET_MEMBER_DEFINE(type)                                                                                            \
static engine_core::AssetType GetStaticAssetType()                                                                                         \
{                                                                                                                                          \
    return engine_core::AssetType(#type);                                                                                                  \
}                                                                                                                                          \
                                                                                                                                           \
static core::TReferencePtr<engine_graphics::GraphicsAssetBase> Create(const engine_core::AssetHeader& header, render::DrawContext& context)   \
{                                                                                                                                          \
    type* asset(new type(header, context));                                                                               \
    return core::MemoryUtilities::CheckedCast<engine_graphics::GraphicsAssetBase*, type*>(asset);                                                           \
}                                                                                                                                          \


namespace slug::engine_graphics
{

class GraphicsAssetBase : public engine_core::AssetBase
{
public:
    GraphicsAssetBase(const engine_core::AssetHeader& header, render::DrawContext& context);

protected:
    render::DrawContext& m_context;
};

using GraphicsAssetBasePtr = core::TReferencePtr<GraphicsAssetBase>;

template<typename T>
concept DerivedFromGraphicsAssetBase = std::is_base_of_v<GraphicsAssetBase, T>;

}
