#pragma once

#include "render/context/DrawContext.hpp"
#include "render/texture/Texture.hpp"

namespace slug::engine_graphics
{

class TextureParser
{
public:
    struct InputParam
    {
        void* data = nullptr;
        size_t size = 0;
        render::DrawContext* context = nullptr;
    };

    struct OutParam
    {
        render::TexturePtr texture = nullptr;
    };

    static bool Parse(InputParam& input, OutParam& out);

};

}
