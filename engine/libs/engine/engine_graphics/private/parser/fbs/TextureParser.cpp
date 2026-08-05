#include "engine_graphics/parser/TextureParser.hpp"
#include <srtf/Texture_fbs.h>
#include <image/ImageUtility.hpp>

namespace slug::engine_graphics
{

struct SamplerAddressModeMapping
{
    render_core::SamplerAddressMode samplerAddressMode;
    fbs::srtf::SamplerAddressMode srtfSamplerAddressMode;
};

struct SamplerReductionTypeMapping
{
    render_core::SamplerReductionType samplerReductionType;
    fbs::srtf::SamplerReductionType srtfSamplerReductionType;
};

static SamplerAddressModeMapping SAMPLER_ADDRESS_MODE_MAPPING[] =
{
    {render_core::SamplerAddressMode::Clamp, fbs::srtf::SamplerAddressMode_Clamp},
    {render_core::SamplerAddressMode::Wrap, fbs::srtf::SamplerAddressMode_Wrap},
    {render_core::SamplerAddressMode::Border, fbs::srtf::SamplerAddressMode_Border},
    {render_core::SamplerAddressMode::Mirror, fbs::srtf::SamplerAddressMode_Mirror},
};

static SamplerReductionTypeMapping SAMPLER_REDUCTION_TYPE_MAPPING[] =
{
    {render_core::SamplerReductionType::Standard, fbs::srtf::SamplerReductionType_Standard},
    {render_core::SamplerReductionType::Comparison, fbs::srtf::SamplerReductionType_Comparison},
    {render_core::SamplerReductionType::Minimum, fbs::srtf::SamplerReductionType_Minimum},
    {render_core::SamplerReductionType::Maximum, fbs::srtf::SamplerReductionType_Maximum},
};

render_core::SamplerAddressMode ConvertSamplerAddressMode(fbs::srtf::SamplerAddressMode mode)
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

render_core::SamplerReductionType ConvertSamplerReductionType(fbs::srtf::SamplerReductionType type)
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

void ParseSampler(render_core::SamplerInfo& samplerInfo, const fbs::srtf::Sampler* fbsSampler)
{

}

void ParseTextureInfo(render_core::TextureInfo& textureInfo, const image::ImageUtility::ImageInfo& srcImageInfo)
{

}

render_core::TexturePtr ParseTexture(render_core::RenderService& service, const fbs::srtf::Texture* fbsTexture)
{
    image::ImageUtility::InputParam input;
    input.data = (void*)fbsTexture->source()->data();
    input.size = fbsTexture->source()->size();

    image::ImageUtility::OutParam out;
    image::ImageUtility::Parse(input, out);

    render_core::Texture::SetupParameter param;
    ParseSampler(param.samplerInfo, fbsTexture->sampler());
    ParseTextureInfo(param.textureInfo, out.imageInfo);
    param.textureData = out.imageInfo.data;
    param.write = true;

    return render_core::Texture::Create(context, param);
}

bool TextureParser::Parse(InputParam& input, OutParam& out)
{
    auto fbsTexture = fbs::srtf::GetTexture(input.data);
    out.texture = ParseTexture(*input.context, fbsTexture);
    return true;
}

}
