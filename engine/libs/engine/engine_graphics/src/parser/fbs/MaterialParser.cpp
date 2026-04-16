#include "engine_graphics/parser/MaterialParser.hpp"
#include "engine_graphics//asset/TextureAsset.hpp"
#include "engine_graphics/asset/ShaderAsset.hpp"
#include <srtf/Shader_fbs.h>
#include <srtf/Materials_fbs.h>

namespace slug::engine_graphics
{

struct BlendOptionsMapping
{
    render::BlendOptions blendOptions;
    fbs::srtf::BlendOptions fbsBlendOptions;
};

static BlendOptionsMapping BLEND_OPTIONS_MAPPING[] =
{
    {render::BlendOptions::Opaque, fbs::srtf::BlendOptions::BlendOptions_Opaque},
    {render::BlendOptions::Mask, fbs::srtf::BlendOptions::BlendOptions_Mask},
    {render::BlendOptions::Transparent, fbs::srtf::BlendOptions::BlendOptions_Transparent},
    {render::BlendOptions::Addtive, fbs::srtf::BlendOptions::BlendOptions_Addtive},
    {render::BlendOptions::Modulate, fbs::srtf::BlendOptions::BlendOptions_Modulate}
};

render::BlendOptions ConverteBlendOptions(const fbs::srtf::BlendOptions options)
{
    for (auto& mapping : BLEND_OPTIONS_MAPPING)
    {
        if (mapping.fbsBlendOptions == options)
        {
            return mapping.blendOptions;
        }
    }
    return BLEND_OPTIONS_MAPPING[0].blendOptions;
}

void ParseConstantParameter(render::ConstantParameter* dstConstantParameter, const fbs::srtf::ConstantParameter* srcConstantParameter)
{
    for (uint32_t i = 0; i < srcConstantParameter->layouts()->size(); i++)
    {
        auto layout = srcConstantParameter->layouts()->Get(i);
        if (layout->type() == fbs::srtf::ConstantParameterType_Bool)
        {
            for (uint32_t j = 0; j < srcConstantParameter->bool_parameters()->size(); j++)
            {
                auto parameter = srcConstantParameter->bool_parameters()->Get(j);
                if (parameter->name() == layout->parameter_name())
                {
                    dstConstantParameter->AddParameter(parameter->name()->c_str(), parameter->value());
                }
            }
        } else if (layout->type() == fbs::srtf::ConstantParameterType_Float)
        {
            for (uint32_t j = 0; j < srcConstantParameter->float_parameters()->size(); j++)
            {
                auto parameter = srcConstantParameter->float_parameters()->Get(j);
                if (parameter->name() == layout->parameter_name())
                {
                    dstConstantParameter->AddParameter(parameter->name()->c_str(), parameter->value());
                }
            }
        } else if (layout->type() == fbs::srtf::ConstantParameterType_FloatArray)
        {
            for (uint32_t j = 0; j < srcConstantParameter->float_array_parameters()->size(); j++)
            {
                auto parameter = srcConstantParameter->float_array_parameters()->Get(j);
                if (parameter->name() == layout->parameter_name())
                {
                    dstConstantParameter->AddParameter(parameter->name()->c_str(), parameter->value()->data(), (size_t)parameter->value()->size());
                }
            }
        } else if (layout->type() == fbs::srtf::ConstantParameterType_Vector2)
        {
            for (uint32_t j = 0; j < srcConstantParameter->vector2_parameters()->size(); j++)
            {
                auto parameter = srcConstantParameter->vector2_parameters()->Get(j);
                if (parameter->name() == layout->parameter_name())
                {
                    math::Vector2 value(parameter->value()->x(), parameter->value()->y());
                    dstConstantParameter->AddParameter(parameter->name()->c_str(), value);
                }
            }
        } else if (layout->type() == fbs::srtf::ConstantParameterType_Vector3)
        {
            for (uint32_t j = 0; j < srcConstantParameter->vector3_parameters()->size(); j++)
            {
                auto parameter = srcConstantParameter->vector3_parameters()->Get(j);
                if (parameter->name() == layout->parameter_name())
                {
                    math::Vector3 value(parameter->value()->x(), parameter->value()->y(), parameter->value()->z());
                    dstConstantParameter->AddParameter(parameter->name()->c_str(), value);
                }
            }
        } else if (layout->type() == fbs::srtf::ConstantParameterType_Vector4)
        {
            for (uint32_t j = 0; j < srcConstantParameter->vector4_parameters()->size(); j++)
            {
                auto parameter = srcConstantParameter->vector4_parameters()->Get(j);
                if (parameter->name() == layout->parameter_name())
                {
                    math::Vector4 value(parameter->value()->x(), parameter->value()->y(), parameter->value()->z(), parameter->value()->w());
                    dstConstantParameter->AddParameter(parameter->name()->c_str(), value);
                }
            }
        } else if (layout->type() == fbs::srtf::ConstantParameterType_Matrix3x3)
        {
            for (uint32_t j = 0; j < srcConstantParameter->matrix3x3_parameters()->size(); j++)
            {
                auto parameter = srcConstantParameter->matrix3x3_parameters()->Get(j);
                if (parameter->name() == layout->parameter_name())
                {
                    math::Matrix3x3 value;
                    for (uint32_t k = 0; k < 9; k++)
                    {
                        value.m[k] = parameter->value()->m()->Get(k);
                    }
                    dstConstantParameter->AddParameter(parameter->name()->c_str(), value);
                }
            }
        } else if (layout->type() == fbs::srtf::ConstantParameterType_Matrix4x4)
        {
            for (uint32_t j = 0; j < srcConstantParameter->matrix4x4_parameters()->size(); j++)
            {
                auto parameter = srcConstantParameter->matrix4x4_parameters()->Get(j);
                if (parameter->name() == layout->parameter_name())
                {
                    math::Matrix4x4 value;
                    for (uint32_t k = 0; k < 16; k++)
                    {
                        value.m[k] = parameter->value()->m()->Get(k);
                    }
                    dstConstantParameter->AddParameter(parameter->name()->c_str(), value);
                }
            }
        }
    }
}

void ParseTextureSlot(core::TVector<ShaderParser::TextureSlot>& textureSlots, const flatbuffers::Vector<::flatbuffers::Offset<slug::fbs::srtf::TextureParameter>>* textures)
{
    for (uint32_t i = 0; i < textures->size(); i++)
    {
        auto srcTexture = textures->Get(i);
        auto& dstTexture = textureSlots.emplace_back();
        dstTexture.slotName = srcTexture->slot_name()->c_str();
        dstTexture.textureId.type = engine_core::AssetType(engine_graphics::TextureAsset::GetStaticAssetType());
        dstTexture.textureId.path = engine_core::AssetPath(srcTexture->texture()->c_str());
    }
}

void ParseStaticSwitches(render::ShaderParameter* dstShader, const flatbuffers::Vector<::flatbuffers::Offset<slug::fbs::srtf::BoolParameter>>* staticSwitches)
{
    for (uint32_t i = 0; i < staticSwitches->size(); i++)
    {
        auto staticSwitch = staticSwitches->Get(i);
        dstShader->AddStaticSwitches(staticSwitch->name()->c_str(), staticSwitch->value());
    }
}

void ParseShaderItem(render::DrawContext& context, render::ShaderPtr& dstShader, const fbs::srtf::ShaderItem* shaderItem, render::ShaderType type, core::StringView shaderName)
{
    render::Shader::SetupParam param;
    param.type = type;
    param.shaderName = shaderName;
    param.variationName = shaderItem->name()->c_str();
    param.code = (void*)shaderItem->code()->data();
    param.size = shaderItem->code()->size();
    dstShader->AddShader(context, param);
}

void ParseShaderItems(render::DrawContext& context, render::ShaderPtr& dstShader, const fbs::srtf::Shader* shader)
{
    for (uint32_t i = 0; i < shader->vertex_shaders()->size(); i++)
    {
        ParseShaderItem(context, dstShader, shader->vertex_shaders()->Get(i), render::ShaderType::Vertex, shader->name()->c_str());
    }

    for (uint32_t i = 0; i < shader->pixel_shaders()->size(); i++)
    {
        ParseShaderItem(context, dstShader, shader->pixel_shaders()->Get(i), render::ShaderType::Pixel, shader->name()->c_str());
    }

    for (uint32_t i = 0; i < shader->geometry_shaders()->size(); i++)
    {
        ParseShaderItem(context, dstShader, shader->geometry_shaders()->Get(i), render::ShaderType::Geometry, shader->name()->c_str());
    }

    for (uint32_t i = 0; i < shader->hull_shaders()->size(); i++)
    {
        ParseShaderItem(context, dstShader, shader->hull_shaders()->Get(i), render::ShaderType::Hull, shader->name()->c_str());
    }

    for (uint32_t i = 0; i < shader->domain_shaders()->size(); i++)
    {
        ParseShaderItem(context, dstShader, shader->domain_shaders()->Get(i), render::ShaderType::Domain, shader->name()->c_str());
    }

    for (uint32_t i = 0; i < shader->compute_shaders()->size(); i++)
    {
        ParseShaderItem(context, dstShader, shader->compute_shaders()->Get(i), render::ShaderType::Compute, shader->name()->c_str());
    }
}

void ParseDrawSettings(render::ShaderParameter* dstShader, const fbs::srtf::DrawSettings* drawSettings)
{
    render::DrawSettings dstDrawSettings = {};
    dstDrawSettings.blendOptions = ConverteBlendOptions(drawSettings->blend());
    dstDrawSettings.shadingModel = render::ShadingModel { drawSettings->shading_model()->name()->c_str(), drawSettings->shading_model()->id() };
    dstDrawSettings.giSettings = render::GiSettings { drawSettings->gi_settings()->diffuse_boost(), drawSettings->gi_settings()->emissive_boost() };
    dstDrawSettings.alphaThreshold = drawSettings->alpha_threshold();
    dstDrawSettings.sortPriority = drawSettings->sort_priority();
    dstDrawSettings.twoSide = drawSettings->two_side();
    dstDrawSettings.alphaToCoverage = drawSettings->alpha_to_coverage();
    dstShader->SetDrawSetttings(dstDrawSettings);
}

bool ShaderParser::Load(const InputParam& input, OutParam& out)
{
    auto fbsShader = fbs::srtf::GetShader(input.data);
    out.shader = core::MakeReference<render::Shader>();

    ParseDrawSettings(out.shader.get(), fbsShader->draw_settings());
    ParseConstantParameter(out.shader.get(), fbsShader->constant_parameter());
    ParseTextureSlot(out.textureSlots, fbsShader->textuers());
    ParseStaticSwitches(out.shader.get(), fbsShader->static_swiches());
    ParseShaderItems(*input.context, out.shader, fbsShader);
    return true;
}

bool MaterialParser::Load(const InputParam& input, OutParam& out)
{
    auto fbsMaterial = fbs::srtf::GetMaterial(input.data);
    out.material = core::MakeReference<render::Material>();
    out.material->SetName(fbsMaterial->name()->c_str());

    ParseDrawSettings(out.material.get(), fbsMaterial->draw_settings());
    ParseConstantParameter(out.material.get(), fbsMaterial->constant_parameter());
    ParseTextureSlot(out.textureSlots, fbsMaterial->textuers());
    ParseStaticSwitches(out.material.get(), fbsMaterial->static_swiches());

    out.shaderSlot.shaderId.path = engine_core::AssetPath(fbsMaterial->shader()->c_str());
    out.shaderSlot.shaderId.type = ShaderAsset::GetStaticAssetType();
    return true;
}

}
