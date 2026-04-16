#include "engine_graphics/parser/TextureParser.hpp"
#include <srtf/Texture_fbs.h>
#include <image/ImageUtility.hpp>

namespace slug::engine_graphics
{

struct SamplerAddressModeMapping
{
    render::SamplerAddressMode samplerAddressMode;
    fbs::srtf::SamplerAddressMode srtfSamplerAddressMode;
};

struct SamplerReductionTypeMapping
{
    render::SamplerReductionType samplerReductionType;
    fbs::srtf::SamplerReductionType srtfSamplerReductionType;
};

static SamplerAddressModeMapping SAMPLER_ADDRESS_MODE_MAPPING[] =
{
    {render::SamplerAddressMode::Clamp, fbs::srtf::SamplerAddressMode_Clamp},
    {render::SamplerAddressMode::Wrap, fbs::srtf::SamplerAddressMode_Wrap},
    {render::SamplerAddressMode::Border, fbs::srtf::SamplerAddressMode_Border},
    {render::SamplerAddressMode::Mirror, fbs::srtf::SamplerAddressMode_Mirror},
};

static SamplerReductionTypeMapping SAMPLER_REDUCTION_TYPE_MAPPING[] =
{
    {render::SamplerReductionType::Standard, fbs::srtf::SamplerReductionType_Standard},
    {render::SamplerReductionType::Comparison, fbs::srtf::SamplerReductionType_Comparison},
    {render::SamplerReductionType::Minimum, fbs::srtf::SamplerReductionType_Minimum},
    {render::SamplerReductionType::Maximum, fbs::srtf::SamplerReductionType_Maximum},
};

render::SamplerAddressMode ConvertSamplerAddressMode(fbs::srtf::SamplerAddressMode mode)
{
    for(auto& mapping : SAMPLER_ADDRESS_MODE_MAPPING)
    {
        if (mapping.srtfSamplerAddressMode == mode)
        {
            return mapping.samplerAddressMode;
        }
    }
    return SAMPLER_ADDRESS_MODE_MAPPING[0].samplerAddressMode;
}

render::SamplerReductionType ConvertSamplerReductionType(fbs::srtf::SamplerReductionType type)
{
    for(auto& mapping : SAMPLER_REDUCTION_TYPE_MAPPING)
    {
        if (mapping.srtfSamplerReductionType == type)
        {
            return mapping.samplerReductionType;
        }
    }
    return SAMPLER_REDUCTION_TYPE_MAPPING[0].samplerReductionType;
}

void ParseSampler(render::SamplerInfo& samplerInfo, const fbs::srtf::Sampler* fbsSampler)
{

}

void ParseTextureInfo(render::TextureInfo& textureInfo, const image::ImageUtility::ImageInfo& srcImageInfo)
{

}

render::TexturePtr ParseTexture(render::DrawContext& context, const fbs::srtf::Texture* fbsTexture)
{
    image::ImageUtility::InputParam input;
    input.data = (void*)fbsTexture->source()->data();
    input.size = fbsTexture->source()->size();

    image::ImageUtility::OutParam out;
    image::ImageUtility::Parse(input, out);

    render::Texture::SetupParameter param;
    ParseSampler(param.samplerInfo, fbsTexture->sampler());
    ParseTextureInfo(param.textureInfo, out.imageInfo);
    param.textureData = out.imageInfo.data;
    param.write = true;

    return render::Texture::Create(context, param);
}

bool TextureParser::Parse(InputParam& input, OutParam& out)
{
    auto fbsTexture = fbs::srtf::GetTexture(input.data);
    out.texture = ParseTexture(*input.context, fbsTexture);
    return true;
}

}
