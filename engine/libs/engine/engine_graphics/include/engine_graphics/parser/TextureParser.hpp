#pragma once

#include "render_core/service/RenderService.hpp"
#include "render_core/texture/Texture.hpp"

namespace slug::engine_graphics
{

class TextureParser
{
public:
    struct InputParam
    {
        void* data = nullptr;
        size_t size = 0;
        render_core::DrawContext* context = nullptr;
    };

    struct OutParam
    {
        render_core::TexturePtr texture = nullptr;
    };

    static bool Parse(InputParam& input, OutParam& out);

};

}
