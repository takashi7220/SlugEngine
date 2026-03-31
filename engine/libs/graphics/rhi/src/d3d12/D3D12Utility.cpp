#pragma once

#include "rhi/d3d12/D3D12Utility.hpp"
#include "rhi/common/Utility.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

static const DxgiFormatMapping DXGI_FORMAT_MAPPING[] = 
{
    { FormatType::UNKNOWN,              DXGI_FORMAT_UNKNOWN,                DXGI_FORMAT_UNKNOWN,                  DXGI_FORMAT_UNKNOWN                },

    { FormatType::R8_UINT,              DXGI_FORMAT_R8_TYPELESS,            DXGI_FORMAT_R8_UINT,                  DXGI_FORMAT_R8_UINT                },
    { FormatType::R8_SINT,              DXGI_FORMAT_R8_TYPELESS,            DXGI_FORMAT_R8_SINT,                  DXGI_FORMAT_R8_SINT                },
    { FormatType::R8_UNORM,             DXGI_FORMAT_R8_TYPELESS,            DXGI_FORMAT_R8_UNORM,                 DXGI_FORMAT_R8_UNORM               },
    { FormatType::R8_SNORM,             DXGI_FORMAT_R8_TYPELESS,            DXGI_FORMAT_R8_SNORM,                 DXGI_FORMAT_R8_SNORM               },
    { FormatType::RG8_UINT,             DXGI_FORMAT_R8G8_TYPELESS,          DXGI_FORMAT_R8G8_UINT,                DXGI_FORMAT_R8G8_UINT              },
    { FormatType::RG8_SINT,             DXGI_FORMAT_R8G8_TYPELESS,          DXGI_FORMAT_R8G8_SINT,                DXGI_FORMAT_R8G8_SINT              },
    { FormatType::RG8_UNORM,            DXGI_FORMAT_R8G8_TYPELESS,          DXGI_FORMAT_R8G8_UNORM,               DXGI_FORMAT_R8G8_UNORM             },
    { FormatType::RG8_SNORM,            DXGI_FORMAT_R8G8_TYPELESS,          DXGI_FORMAT_R8G8_SNORM,               DXGI_FORMAT_R8G8_SNORM             },
    { FormatType::R16_UINT,             DXGI_FORMAT_R16_TYPELESS,           DXGI_FORMAT_R16_UINT,                 DXGI_FORMAT_R16_UINT               },
    { FormatType::R16_SINT,             DXGI_FORMAT_R16_TYPELESS,           DXGI_FORMAT_R16_SINT,                 DXGI_FORMAT_R16_SINT               },
    { FormatType::R16_UNORM,            DXGI_FORMAT_R16_TYPELESS,           DXGI_FORMAT_R16_UNORM,                DXGI_FORMAT_R16_UNORM              },
    { FormatType::R16_SNORM,            DXGI_FORMAT_R16_TYPELESS,           DXGI_FORMAT_R16_SNORM,                DXGI_FORMAT_R16_SNORM              },
    { FormatType::R16_FLOAT,            DXGI_FORMAT_R16_TYPELESS,           DXGI_FORMAT_R16_FLOAT,                DXGI_FORMAT_R16_FLOAT              },
    { FormatType::BGRA4_UNORM,          DXGI_FORMAT_B4G4R4A4_UNORM,         DXGI_FORMAT_B4G4R4A4_UNORM,           DXGI_FORMAT_B4G4R4A4_UNORM         },
    { FormatType::B5G6R5_UNORM,         DXGI_FORMAT_B5G6R5_UNORM,           DXGI_FORMAT_B5G6R5_UNORM,             DXGI_FORMAT_B5G6R5_UNORM           },
    { FormatType::B5G5R5A1_UNORM,       DXGI_FORMAT_B5G5R5A1_UNORM,         DXGI_FORMAT_B5G5R5A1_UNORM,           DXGI_FORMAT_B5G5R5A1_UNORM         },
    { FormatType::RGBA8_UINT,           DXGI_FORMAT_R8G8B8A8_TYPELESS,      DXGI_FORMAT_R8G8B8A8_UINT,            DXGI_FORMAT_R8G8B8A8_UINT          },
    { FormatType::RGBA8_SINT,           DXGI_FORMAT_R8G8B8A8_TYPELESS,      DXGI_FORMAT_R8G8B8A8_SINT,            DXGI_FORMAT_R8G8B8A8_SINT          },
    { FormatType::RGBA8_UNORM,          DXGI_FORMAT_R8G8B8A8_TYPELESS,      DXGI_FORMAT_R8G8B8A8_UNORM,           DXGI_FORMAT_R8G8B8A8_UNORM         },
    { FormatType::RGBA8_SNORM,          DXGI_FORMAT_R8G8B8A8_TYPELESS,      DXGI_FORMAT_R8G8B8A8_SNORM,           DXGI_FORMAT_R8G8B8A8_SNORM         },
    { FormatType::BGRA8_UNORM,          DXGI_FORMAT_B8G8R8A8_TYPELESS,      DXGI_FORMAT_B8G8R8A8_UNORM,           DXGI_FORMAT_B8G8R8A8_UNORM         },
    { FormatType::SRGBA8_UNORM,         DXGI_FORMAT_R8G8B8A8_TYPELESS,      DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,      DXGI_FORMAT_R8G8B8A8_UNORM_SRGB    },
    { FormatType::SBGRA8_UNORM,         DXGI_FORMAT_B8G8R8A8_TYPELESS,      DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,      DXGI_FORMAT_B8G8R8A8_UNORM_SRGB    },
    { FormatType::R10G10B10A2_UNORM,    DXGI_FORMAT_R10G10B10A2_TYPELESS,   DXGI_FORMAT_R10G10B10A2_UNORM,        DXGI_FORMAT_R10G10B10A2_UNORM      },
    { FormatType::R11G11B10_FLOAT,      DXGI_FORMAT_R11G11B10_FLOAT,        DXGI_FORMAT_R11G11B10_FLOAT,          DXGI_FORMAT_R11G11B10_FLOAT        },
    { FormatType::RG16_UINT,            DXGI_FORMAT_R16G16_TYPELESS,        DXGI_FORMAT_R16G16_UINT,              DXGI_FORMAT_R16G16_UINT            },
    { FormatType::RG16_SINT,            DXGI_FORMAT_R16G16_TYPELESS,        DXGI_FORMAT_R16G16_SINT,              DXGI_FORMAT_R16G16_SINT            },
    { FormatType::RG16_UNORM,           DXGI_FORMAT_R16G16_TYPELESS,        DXGI_FORMAT_R16G16_UNORM,             DXGI_FORMAT_R16G16_UNORM           },
    { FormatType::RG16_SNORM,           DXGI_FORMAT_R16G16_TYPELESS,        DXGI_FORMAT_R16G16_SNORM,             DXGI_FORMAT_R16G16_SNORM           },
    { FormatType::RG16_FLOAT,           DXGI_FORMAT_R16G16_TYPELESS,        DXGI_FORMAT_R16G16_FLOAT,             DXGI_FORMAT_R16G16_FLOAT           },
    { FormatType::R32_UINT,             DXGI_FORMAT_R32_TYPELESS,           DXGI_FORMAT_R32_UINT,                 DXGI_FORMAT_R32_UINT               },
    { FormatType::R32_SINT,             DXGI_FORMAT_R32_TYPELESS,           DXGI_FORMAT_R32_SINT,                 DXGI_FORMAT_R32_SINT               },
    { FormatType::R32_FLOAT,            DXGI_FORMAT_R32_TYPELESS,           DXGI_FORMAT_R32_FLOAT,                DXGI_FORMAT_R32_FLOAT              },
    { FormatType::RGBA16_UINT,          DXGI_FORMAT_R16G16B16A16_TYPELESS,  DXGI_FORMAT_R16G16B16A16_UINT,        DXGI_FORMAT_R16G16B16A16_UINT      },
    { FormatType::RGBA16_SINT,          DXGI_FORMAT_R16G16B16A16_TYPELESS,  DXGI_FORMAT_R16G16B16A16_SINT,        DXGI_FORMAT_R16G16B16A16_SINT      },
    { FormatType::RGBA16_FLOAT,         DXGI_FORMAT_R16G16B16A16_TYPELESS,  DXGI_FORMAT_R16G16B16A16_FLOAT,       DXGI_FORMAT_R16G16B16A16_FLOAT     },
    { FormatType::RGBA16_UNORM,         DXGI_FORMAT_R16G16B16A16_TYPELESS,  DXGI_FORMAT_R16G16B16A16_UNORM,       DXGI_FORMAT_R16G16B16A16_UNORM     },
    { FormatType::RGBA16_SNORM,         DXGI_FORMAT_R16G16B16A16_TYPELESS,  DXGI_FORMAT_R16G16B16A16_SNORM,       DXGI_FORMAT_R16G16B16A16_SNORM     },
    { FormatType::RG32_UINT,            DXGI_FORMAT_R32G32_TYPELESS,        DXGI_FORMAT_R32G32_UINT,              DXGI_FORMAT_R32G32_UINT            },
    { FormatType::RG32_SINT,            DXGI_FORMAT_R32G32_TYPELESS,        DXGI_FORMAT_R32G32_SINT,              DXGI_FORMAT_R32G32_SINT            },
    { FormatType::RG32_FLOAT,           DXGI_FORMAT_R32G32_TYPELESS,        DXGI_FORMAT_R32G32_FLOAT,             DXGI_FORMAT_R32G32_FLOAT           },
    { FormatType::RGB32_UINT,           DXGI_FORMAT_R32G32B32_TYPELESS,     DXGI_FORMAT_R32G32B32_UINT,           DXGI_FORMAT_R32G32B32_UINT         },
    { FormatType::RGB32_SINT,           DXGI_FORMAT_R32G32B32_TYPELESS,     DXGI_FORMAT_R32G32B32_SINT,           DXGI_FORMAT_R32G32B32_SINT         },
    { FormatType::RGB32_FLOAT,          DXGI_FORMAT_R32G32B32_TYPELESS,     DXGI_FORMAT_R32G32B32_FLOAT,          DXGI_FORMAT_R32G32B32_FLOAT        },
    { FormatType::RGBA32_UINT,          DXGI_FORMAT_R32G32B32A32_TYPELESS,  DXGI_FORMAT_R32G32B32A32_UINT,        DXGI_FORMAT_R32G32B32A32_UINT      },
    { FormatType::RGBA32_SINT,          DXGI_FORMAT_R32G32B32A32_TYPELESS,  DXGI_FORMAT_R32G32B32A32_SINT,        DXGI_FORMAT_R32G32B32A32_SINT      },
    { FormatType::RGBA32_FLOAT,         DXGI_FORMAT_R32G32B32A32_TYPELESS,  DXGI_FORMAT_R32G32B32A32_FLOAT,       DXGI_FORMAT_R32G32B32A32_FLOAT     },

    { FormatType::D16,                  DXGI_FORMAT_R16_TYPELESS,           DXGI_FORMAT_R16_UNORM,                DXGI_FORMAT_D16_UNORM              },
    { FormatType::D24S8,                DXGI_FORMAT_R24G8_TYPELESS,         DXGI_FORMAT_R24_UNORM_X8_TYPELESS,    DXGI_FORMAT_D24_UNORM_S8_UINT      },
    { FormatType::X24G8_UINT,           DXGI_FORMAT_R24G8_TYPELESS,         DXGI_FORMAT_X24_TYPELESS_G8_UINT,     DXGI_FORMAT_D24_UNORM_S8_UINT      },
    { FormatType::D32,                  DXGI_FORMAT_R32_TYPELESS,           DXGI_FORMAT_R32_FLOAT,                DXGI_FORMAT_D32_FLOAT              },
    { FormatType::D32S8,                DXGI_FORMAT_R32G8X24_TYPELESS,      DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT_D32_FLOAT_S8X24_UINT   },
    { FormatType::X32G8_UINT,           DXGI_FORMAT_R32G8X24_TYPELESS,      DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,  DXGI_FORMAT_D32_FLOAT_S8X24_UINT   },

    { FormatType::BC1_UNORM,            DXGI_FORMAT_BC1_TYPELESS,           DXGI_FORMAT_BC1_UNORM,                DXGI_FORMAT_BC1_UNORM              },
    { FormatType::BC1_UNORM_SRGB,       DXGI_FORMAT_BC1_TYPELESS,           DXGI_FORMAT_BC1_UNORM_SRGB,           DXGI_FORMAT_BC1_UNORM_SRGB         },
    { FormatType::BC2_UNORM,            DXGI_FORMAT_BC2_TYPELESS,           DXGI_FORMAT_BC2_UNORM,                DXGI_FORMAT_BC2_UNORM              },
    { FormatType::BC2_UNORM_SRGB,       DXGI_FORMAT_BC2_TYPELESS,           DXGI_FORMAT_BC2_UNORM_SRGB,           DXGI_FORMAT_BC2_UNORM_SRGB         },
    { FormatType::BC3_UNORM,            DXGI_FORMAT_BC3_TYPELESS,           DXGI_FORMAT_BC3_UNORM,                DXGI_FORMAT_BC3_UNORM              },
    { FormatType::BC3_UNORM_SRGB,       DXGI_FORMAT_BC3_TYPELESS,           DXGI_FORMAT_BC3_UNORM_SRGB,           DXGI_FORMAT_BC3_UNORM_SRGB         },
    { FormatType::BC4_UNORM,            DXGI_FORMAT_BC4_TYPELESS,           DXGI_FORMAT_BC4_UNORM,                DXGI_FORMAT_BC4_UNORM              },
    { FormatType::BC4_SNORM,            DXGI_FORMAT_BC4_TYPELESS,           DXGI_FORMAT_BC4_SNORM,                DXGI_FORMAT_BC4_SNORM              },
    { FormatType::BC5_UNORM,            DXGI_FORMAT_BC5_TYPELESS,           DXGI_FORMAT_BC5_UNORM,                DXGI_FORMAT_BC5_UNORM              },
    { FormatType::BC5_SNORM,            DXGI_FORMAT_BC5_TYPELESS,           DXGI_FORMAT_BC5_SNORM,                DXGI_FORMAT_BC5_SNORM              },
    { FormatType::BC6H_UFLOAT,          DXGI_FORMAT_BC6H_TYPELESS,          DXGI_FORMAT_BC6H_UF16,                DXGI_FORMAT_BC6H_UF16              },
    { FormatType::BC6H_SFLOAT,          DXGI_FORMAT_BC6H_TYPELESS,          DXGI_FORMAT_BC6H_SF16,                DXGI_FORMAT_BC6H_SF16              },
    { FormatType::BC7_UNORM,            DXGI_FORMAT_BC7_TYPELESS,           DXGI_FORMAT_BC7_UNORM,                DXGI_FORMAT_BC7_UNORM              },
    { FormatType::BC7_UNORM_SRGB,       DXGI_FORMAT_BC7_TYPELESS,           DXGI_FORMAT_BC7_UNORM_SRGB,           DXGI_FORMAT_BC7_UNORM_SRGB         },
};

static const ShaderVisibilityMapping SHADER_VISIBILITY_MAPPING[] =
{
    {ShaderType::All, D3D12_SHADER_VISIBILITY_ALL },
    {ShaderType::Vertex, D3D12_SHADER_VISIBILITY_VERTEX},
    {ShaderType::Hull, D3D12_SHADER_VISIBILITY_HULL},
    {ShaderType::Domain, D3D12_SHADER_VISIBILITY_DOMAIN},
    {ShaderType::Geometry, D3D12_SHADER_VISIBILITY_GEOMETRY},
    {ShaderType::Pixel, D3D12_SHADER_VISIBILITY_PIXEL},
    {ShaderType::Amplification, D3D12_SHADER_VISIBILITY_AMPLIFICATION},
    {ShaderType::Mesh, D3D12_SHADER_VISIBILITY_MESH},
};

static const BlendMapping BLEND_MAPPING[] =
{
    {BlendFactor::Zero, D3D12_BLEND_ZERO},
    {BlendFactor::One, D3D12_BLEND_ONE},
    {BlendFactor::SrcColor, D3D12_BLEND_SRC_COLOR},
    {BlendFactor::InvSrcColor, D3D12_BLEND_INV_SRC_COLOR},
    {BlendFactor::SrcAlpha, D3D12_BLEND_SRC_ALPHA},
    {BlendFactor::InvSrcAlpha, D3D12_BLEND_INV_SRC_ALPHA},
    {BlendFactor::DstAlpha, D3D12_BLEND_DEST_ALPHA},
    {BlendFactor::InvDstAlpha, D3D12_BLEND_INV_DEST_ALPHA},
    {BlendFactor::DstColor, D3D12_BLEND_DEST_COLOR},
    {BlendFactor::InvDstColor, D3D12_BLEND_INV_DEST_COLOR},
    {BlendFactor::SrcAlphaSaturate, D3D12_BLEND_SRC_ALPHA_SAT},
    {BlendFactor::ConstantColor, D3D12_BLEND_BLEND_FACTOR},
    {BlendFactor::InvConstantColor, D3D12_BLEND_INV_BLEND_FACTOR},
    {BlendFactor::Src1Color, D3D12_BLEND_SRC1_COLOR},
    {BlendFactor::InvSrc1Color, D3D12_BLEND_INV_SRC1_COLOR},
    {BlendFactor::Src1Alpha, D3D12_BLEND_SRC1_ALPHA},
    {BlendFactor::InvSrc1Alpha, D3D12_BLEND_INV_SRC1_ALPHA},
};

static const BlendOpMapping BLEND_OP_MAPPING[] =
{
    {BlendOp::Add, D3D12_BLEND_OP_ADD},
    {BlendOp::Max, D3D12_BLEND_OP_MAX},
    {BlendOp::Min, D3D12_BLEND_OP_MIN},
    {BlendOp::ReverseSubtract, D3D12_BLEND_OP_REV_SUBTRACT},
    {BlendOp::Subrtact, D3D12_BLEND_OP_SUBTRACT},
};

static const StencilOpMapping STENCIL_OP_MAPPING[] =
{
    {StencilOp::Keep, D3D12_STENCIL_OP_KEEP},
    {StencilOp::Zero, D3D12_STENCIL_OP_ZERO},
    {StencilOp::Replace, D3D12_STENCIL_OP_REPLACE},
    {StencilOp::IncrementAndClamp, D3D12_STENCIL_OP_INCR_SAT},
    {StencilOp::DecrementAndClamp, D3D12_STENCIL_OP_DECR_SAT},
    {StencilOp::Invert, D3D12_STENCIL_OP_INVERT},
    {StencilOp::IncrementAndWrap, D3D12_STENCIL_OP_INCR},
    {StencilOp::DecrementAndWrap, D3D12_STENCIL_OP_DECR},
};

static const ComparisonFuncMapping COMPARISON_FUNC_MAPPING[] =
{
    {ComparisonFunc::Never, D3D12_COMPARISON_FUNC_NEVER},
    {ComparisonFunc::Less, D3D12_COMPARISON_FUNC_LESS},
    {ComparisonFunc::Equal, D3D12_COMPARISON_FUNC_EQUAL},
    {ComparisonFunc::LessOrEqual, D3D12_COMPARISON_FUNC_LESS_EQUAL},
    {ComparisonFunc::Greater, D3D12_COMPARISON_FUNC_GREATER},
    {ComparisonFunc::GreaterOrEqual, D3D12_COMPARISON_FUNC_GREATER_EQUAL},
    {ComparisonFunc::NotEqual, D3D12_COMPARISON_FUNC_NOT_EQUAL},
    {ComparisonFunc::Always, D3D12_COMPARISON_FUNC_ALWAYS},
};

static const PrimitiveTypeMapping PRIMITIVE_TYPE_MAPPING[] =
{
    {PrimitiveType::Undifine, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED},
    {PrimitiveType::PointList, D3D_PRIMITIVE_TOPOLOGY_POINTLIST},
    {PrimitiveType::LineList, D3D_PRIMITIVE_TOPOLOGY_LINELIST},
    {PrimitiveType::TriangleList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST},
    {PrimitiveType::TriangleStrip, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP},
    {PrimitiveType::TriangleFan, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP},
    {PrimitiveType::TriangleListWithAdjacency, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ},
    {PrimitiveType::TriangleStripWithAdjacency, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ},
    {PrimitiveType::PatchList, D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST},
};

static const D3D12PrimitiveTypologyTypeMapping D3D12_PRIMITIVE_TOPOLOGY_TYPE_MAPPING[]
{
    {PrimitiveType::PointList, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT},
    {PrimitiveType::LineList, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE},
    {PrimitiveType::TriangleList, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE},
    {PrimitiveType::TriangleStrip, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE},
    {PrimitiveType::PointList, D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH},
};

static const TextureAddressModeMapping TEXTURE_ADDRESS_MODE_MAPPING[] =
{
    {SamplerAddressMode::Clamp, D3D12_TEXTURE_ADDRESS_MODE_CLAMP},
    {SamplerAddressMode::Wrap, D3D12_TEXTURE_ADDRESS_MODE_WRAP},
    {SamplerAddressMode::Border, D3D12_TEXTURE_ADDRESS_MODE_BORDER},
    {SamplerAddressMode::Mirror, D3D12_TEXTURE_ADDRESS_MODE_MIRROR},
    {SamplerAddressMode::MirrorOnce, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE},
};

static const SamplerReductionTypeMapping SAMPLER_REDUCTION_TYPE_MAPPING[] =
{
    {SamplerReductionType::Standard, D3D12_FILTER_REDUCTION_TYPE_STANDARD},
    {SamplerReductionType::Comparison, D3D12_FILTER_REDUCTION_TYPE_COMPARISON},
    {SamplerReductionType::Maximum, D3D12_FILTER_REDUCTION_TYPE_MAXIMUM},
    {SamplerReductionType::Minimum, D3D12_FILTER_REDUCTION_TYPE_MINIMUM},
};

static const VariableShadingRateMapping VARIABLE_SHADING_RATE_MAPPING[] =
{
    {VariableShadingRate::e1x1, D3D12_SHADING_RATE_1X1},
    {VariableShadingRate::e1x2, D3D12_SHADING_RATE_1X2},
    {VariableShadingRate::e2x1, D3D12_SHADING_RATE_2X1},
    {VariableShadingRate::e2x2, D3D12_SHADING_RATE_1X2},
    {VariableShadingRate::e2x4, D3D12_SHADING_RATE_2X4},
    {VariableShadingRate::e4x2, D3D12_SHADING_RATE_4X2},
    {VariableShadingRate::e4x4, D3D12_SHADING_RATE_4X4},
};

static const ShadingRateCombinerMapping SHADING_RATE_COMBINER_MAPPING[] =
{
    {ShadingRateCombiner::Passthrough, D3D12_SHADING_RATE_COMBINER_PASSTHROUGH},
    {ShadingRateCombiner::Override, D3D12_SHADING_RATE_COMBINER_OVERRIDE},
    {ShadingRateCombiner::ApplyRelative, D3D12_SHADING_RATE_COMBINER_SUM},
    {ShadingRateCombiner::Max, D3D12_SHADING_RATE_COMBINER_MAX},
    {ShadingRateCombiner::Min, D3D12_SHADING_RATE_COMBINER_MIN},
};

const DxgiFormatMapping& D3D12Utility::GetDxgiFormatMapping(FormatType abstractFormat)
{
    SLUG_STATIC_ASSERT_MSG(sizeof(DXGI_FORMAT_MAPPING) / sizeof(DxgiFormatMapping) == size_t(FormatType::COUNT),
        "The format mapping table doesn't have the right number of elements");

    const DxgiFormatMapping& mapping = DXGI_FORMAT_MAPPING[uint32_t(abstractFormat)];
    SLUG_ASSERT(mapping.abstractFormat == abstractFormat);
    return mapping;
}

const DxgiFormatMapping& D3D12Utility::GetDxgiFormatMapping(DXGI_FORMAT dxgiFormat, DxgiFormatMapping::UseType useType)
{
    for (auto& mapping : DXGI_FORMAT_MAPPING)
    {
        if (useType == DxgiFormatMapping::UseType::Resource)
        {
            if (mapping.resourceFormat == dxgiFormat)
            {
                return mapping;
            }
        }
        else if (useType == DxgiFormatMapping::UseType::SRV)
        {
            if (mapping.srvFormat == dxgiFormat)
            {
                return mapping;
            }
        }
        else if (useType == DxgiFormatMapping::UseType::RTV)
        {
            if (mapping.rtvFormat == dxgiFormat)
            {
                return mapping;
            }
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "DXGI_FORMAT conversion Failed.");
    return DXGI_FORMAT_MAPPING[0];
}

const ShaderVisibilityMapping& D3D12Utility::GetShaderVisibilityMapping(ShaderType shaderType)
{
    for (auto& mapping : SHADER_VISIBILITY_MAPPING)
    {
        if (mapping.shaderType == shaderType)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "ShaderType conversion Failed.");
    return SHADER_VISIBILITY_MAPPING[0];
}

const ShaderVisibilityMapping& D3D12Utility::GetShaderVisibilityMapping(D3D12_SHADER_VISIBILITY shaderVisibility)
{
    for (auto& mapping : SHADER_VISIBILITY_MAPPING)
    {
        if (mapping.shaderVisibility == shaderVisibility)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_SHADER_VISIBILITY conversion Failed.");
    return SHADER_VISIBILITY_MAPPING[0];
}

const BlendMapping& D3D12Utility::GetBlendMapping(BlendFactor factor)
{
    for (auto& mapping : BLEND_MAPPING)
    {
        if (mapping.blendFactor == factor)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "BlendFactor conversion Failed.");
    return BLEND_MAPPING[0];
}

const BlendMapping& D3D12Utility::GetBlendMapping(D3D12_BLEND blend)
{
    for (auto& mapping : BLEND_MAPPING)
    {
        if (mapping.d3d12Blend == blend)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_BLEND conversion Failed.");
    return BLEND_MAPPING[0];
}

const BlendOpMapping& D3D12Utility::GetBlendOpMapping(BlendOp blendOp)
{
    for (auto& mapping : BLEND_OP_MAPPING)
    {
        if (mapping.blendOp == blendOp)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "BlendOp conversion Failed.");
    return BLEND_OP_MAPPING[0];
}

const BlendOpMapping& D3D12Utility::GetBlendOpMapping(D3D12_BLEND_OP d3d12BlendOp)
{
    for (auto& mapping : BLEND_OP_MAPPING)
    {
        if (mapping.d3d12BlendOp == d3d12BlendOp)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_BLEND_OP conversion Failed.");
    return BLEND_OP_MAPPING[0];
}

const StencilOpMapping& D3D12Utility::GetStencilOpMapping(StencilOp stencilOp)
{
    for (auto& mapping : STENCIL_OP_MAPPING)
    {
        if (mapping.stencilOp == stencilOp)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "StencilOp conversion Failed.");
    return STENCIL_OP_MAPPING[0];
}

const StencilOpMapping& D3D12Utility::GetStencilOpMapping(D3D12_STENCIL_OP stencilOp)
{
    for (auto& mapping : STENCIL_OP_MAPPING)
    {
        if (mapping.d3d12StencilOp == stencilOp)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_STENCIL_OP conversion Failed.");
    return STENCIL_OP_MAPPING[0];
}

const ComparisonFuncMapping& D3D12Utility::GetComparisonFuncMapping(ComparisonFunc comparisonFunc)
{
    for (auto& mapping : COMPARISON_FUNC_MAPPING)
    {
        if (mapping.comparisonFunc == comparisonFunc)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "ComparisonFunc conversion Failed.");
    return COMPARISON_FUNC_MAPPING[0];
}

const ComparisonFuncMapping& D3D12Utility::GetComparisonFuncMapping(D3D12_COMPARISON_FUNC d3d12ComparisonFunc)
{
    for (auto& mapping : COMPARISON_FUNC_MAPPING)
    {
        if (mapping.d3d12ComparisonFunc == d3d12ComparisonFunc)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_COMPARISON_FUNC conversion Failed.");
    return COMPARISON_FUNC_MAPPING[0];
}

const PrimitiveTypeMapping& D3D12Utility::GetPrimitiveTypeMapping(PrimitiveType primitiveType)
{
    for (auto& mapping : PRIMITIVE_TYPE_MAPPING)
    {
        if (mapping.primitiveType == primitiveType)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "PrimitiveType conversion Failed.");
    return PRIMITIVE_TYPE_MAPPING[0];
}

const PrimitiveTypeMapping& D3D12Utility::GetPrimitiveTypeMapping(D3D_PRIMITIVE_TOPOLOGY d3dPrimitiveType)
{
    for (auto& mapping : PRIMITIVE_TYPE_MAPPING)
    {
        if (mapping.d3dPrimitiveType == d3dPrimitiveType)
        {
            return mapping;
        }
    }

    if (d3dPrimitiveType >= D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST &&
        d3dPrimitiveType <= D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST)
    {
        return PRIMITIVE_TYPE_MAPPING[8];
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D_PRIMITIVE_TOPOLOGY conversion Failed.");
    return PRIMITIVE_TYPE_MAPPING[0];
}

const D3D12PrimitiveTypologyTypeMapping& D3D12Utility::GetD3D12PrimitiveTypeMapping(PrimitiveType primitiveType)
{
    for (auto& mapping : D3D12_PRIMITIVE_TOPOLOGY_TYPE_MAPPING)
    {
        if (mapping.primitiveType == primitiveType)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "PrimitiveType conversion Failed.");
    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_MAPPING[0];
}

const D3D12PrimitiveTypologyTypeMapping& D3D12Utility::GetD3D12PrimitiveTypeMapping(D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d12PrimitiveType)
{
    for (auto& mapping : D3D12_PRIMITIVE_TOPOLOGY_TYPE_MAPPING)
    {
        if (mapping.d3d12PrimitiveType == d3d12PrimitiveType)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_PRIMITIVE_TOPOLOGY_TYPE conversion Failed.");
    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_MAPPING[0];
}

const TextureAddressModeMapping& D3D12Utility::GetTextureAddressModeMapping(SamplerAddressMode samplerAddressMode)
{
    for (auto& mapping : TEXTURE_ADDRESS_MODE_MAPPING)
    {
        if (mapping.samplerAddressMode == samplerAddressMode)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "SamplerAddressMode conversion Failed.");
    return TEXTURE_ADDRESS_MODE_MAPPING[0];
}

const TextureAddressModeMapping& D3D12Utility::GetTextureAddressModeMapping(D3D12_TEXTURE_ADDRESS_MODE d3d12TextureAddressMode)
{
    for (auto& mapping : TEXTURE_ADDRESS_MODE_MAPPING)
    {
        if (mapping.d3d12TextureAddressMode == d3d12TextureAddressMode)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_TEXTURE_ADDRESS_MODE conversion Failed.");
    return TEXTURE_ADDRESS_MODE_MAPPING[0];
}

const SamplerReductionTypeMapping& D3D12Utility::GetSamplerReductionTypeMapping(SamplerReductionType samplerReductionType)
{
    for (auto& mapping : SAMPLER_REDUCTION_TYPE_MAPPING)
    {
        if (mapping.samplerReductionType == samplerReductionType)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "SamplerReductionTypeMapping conversion Failed.");
    return SAMPLER_REDUCTION_TYPE_MAPPING[0];
}

const SamplerReductionTypeMapping& D3D12Utility::GetSamplerReductionTypeMapping(D3D12_FILTER_REDUCTION_TYPE d3d12FilterReductionType)
{
    for (auto& mapping : SAMPLER_REDUCTION_TYPE_MAPPING)
    {
        if (mapping.d3d12FilterReductionType == d3d12FilterReductionType)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_FILTER_REDUCTION_TYPE conversion Failed.");
    return SAMPLER_REDUCTION_TYPE_MAPPING[0];
}

const VariableShadingRateMapping& D3D12Utility::GetVariableShadingRateMapping(VariableShadingRate variableShadingRate)
{
    for (auto& mapping : VARIABLE_SHADING_RATE_MAPPING)
    {
        if (mapping.variableShadingRate == variableShadingRate)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "VariableShadingRate conversion Failed.");
    return VARIABLE_SHADING_RATE_MAPPING[0];
}

const VariableShadingRateMapping& D3D12Utility::GetVariableShadingRateMapping(D3D12_SHADING_RATE d3d12ShadingRate)
{
    for (auto& mapping : VARIABLE_SHADING_RATE_MAPPING)
    {
        if (mapping.d3d12ShadingRate == d3d12ShadingRate)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_SHADING_RATE conversion Failed.");
    return VARIABLE_SHADING_RATE_MAPPING[0];
}

const ShadingRateCombinerMapping& D3D12Utility::GetShadingRateCombinerMapping(ShadingRateCombiner shadingRateCombiner)
{
    for (auto& mapping : SHADING_RATE_COMBINER_MAPPING)
    {
        if (mapping.shadingRateCombiner == shadingRateCombiner)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "ShadingRateCombiner conversion Failed.");
    return SHADING_RATE_COMBINER_MAPPING[0];
}

const ShadingRateCombinerMapping& D3D12Utility::GetShadingRateCombinerMapping(D3D12_SHADING_RATE_COMBINER d3d12ShadingRateCombiner)
{
    for (auto& mapping : SHADING_RATE_COMBINER_MAPPING)
    {
        if (mapping.d3d12ShadingRateCombiner == d3d12ShadingRateCombiner)
        {
            return mapping;
        }
    }
    SLUG_LOG(RHILogCategory, core::LogType::Warning, "D3D12_SHADING_RATE_COMBINER conversion Failed.");
    return SHADING_RATE_COMBINER_MAPPING[0];

}

DXGI_FORMAT D3D12Utility::ConvertFormat(FormatType formatType) 
{
    return D3D12Utility::GetDxgiFormatMapping(formatType).srvFormat;
}

FormatType D3D12Utility::ConvertFormat(DXGI_FORMAT formatType, DxgiFormatMapping::UseType useType)
{
    return D3D12Utility::GetDxgiFormatMapping(formatType, useType).abstractFormat;
}

D3D12_SHADER_VISIBILITY D3D12Utility::ConvertShaderStage(ShaderType shaderType) 
{
    return D3D12Utility::GetShaderVisibilityMapping(shaderType).shaderVisibility;
}

ShaderType D3D12Utility::ConvertShaderStage(D3D12_SHADER_VISIBILITY shaderVisibility)
{
    return D3D12Utility::GetShaderVisibilityMapping(shaderVisibility).shaderType;
}

D3D12_BLEND D3D12Utility::ConvertBlendValue(BlendFactor value) 
{
    return D3D12Utility::GetBlendMapping(value).d3d12Blend;
}

BlendFactor D3D12Utility::ConvertBlendValue(D3D12_BLEND value)
{
    return D3D12Utility::GetBlendMapping(value).blendFactor;
}

D3D12_BLEND_OP D3D12Utility::ConvertBlendOp(BlendOp value) 
{
    return D3D12Utility::GetBlendOpMapping(value).d3d12BlendOp;
}

BlendOp D3D12Utility::ConvertBlendOp(D3D12_BLEND_OP value)
{
    return D3D12Utility::GetBlendOpMapping(value).blendOp;
}

D3D12_STENCIL_OP D3D12Utility::ConvertStencilOp(rhi::StencilOp value) 
{
    return D3D12Utility::GetStencilOpMapping(value).d3d12StencilOp;
}

StencilOp D3D12Utility::ConvertStencilOp(D3D12_STENCIL_OP value)
{
    return D3D12Utility::GetStencilOpMapping(value).stencilOp;
}

D3D12_COMPARISON_FUNC D3D12Utility::ConvertComparisonFunc(rhi::ComparisonFunc value)
{
    return D3D12Utility::GetComparisonFuncMapping(value).d3d12ComparisonFunc;
}

rhi::ComparisonFunc D3D12Utility::ConvertComparisonFunc(D3D12_COMPARISON_FUNC value)
{
    return D3D12Utility::GetComparisonFuncMapping(value).comparisonFunc;
}

D3D_PRIMITIVE_TOPOLOGY D3D12Utility::ConvertPrimitiveType(rhi::PrimitiveType value, uint32_t controlPoints)
{
    auto ret = D3D12Utility::GetPrimitiveTypeMapping(value).d3dPrimitiveType;
    return static_cast<D3D_PRIMITIVE_TOPOLOGY>(ret + controlPoints);
}

rhi::PrimitiveType D3D12Utility::ConvertPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY value, uint32_t& controlPoints)
{
    auto ret = D3D12Utility::GetPrimitiveTypeMapping(value).primitiveType;
    if (ret == PrimitiveType::PatchList)
    {
        controlPoints = static_cast<uint32_t>(value - D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST) + 1;
    }
    return ret;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12Utility::ConvertD3D12PrimitiveType(rhi::PrimitiveType value)
{
    return GetD3D12PrimitiveTypeMapping(value).d3d12PrimitiveType;
}

rhi::PrimitiveType D3D12Utility::ConvertD3D12PrimitiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE value)
{
    return GetD3D12PrimitiveTypeMapping(value).primitiveType;
}

D3D12_TEXTURE_ADDRESS_MODE D3D12Utility::ConvertSamplerAddressMode(rhi::SamplerAddressMode value)
{
    return D3D12Utility::GetTextureAddressModeMapping(value).d3d12TextureAddressMode;
}

rhi::SamplerAddressMode D3D12Utility::ConvertSamplerAddressMode(D3D12_TEXTURE_ADDRESS_MODE value)
{
    return D3D12Utility::GetTextureAddressModeMapping(value).samplerAddressMode;
}

UINT D3D12Utility::ConvertSamplerReductionType(rhi::SamplerReductionType value)
{
    return D3D12Utility::GetSamplerReductionTypeMapping(value).d3d12FilterReductionType;
}

rhi::SamplerReductionType D3D12Utility::ConvertSamplerReductionType(UINT value)
{
    return D3D12Utility::GetSamplerReductionTypeMapping(static_cast<D3D12_FILTER_REDUCTION_TYPE>(value)).samplerReductionType;
}


D3D12_SHADING_RATE D3D12Utility::ConvertPixelShadingRate(rhi::VariableShadingRate value)
{
    return D3D12Utility::GetVariableShadingRateMapping(value).d3d12ShadingRate;
}

rhi::VariableShadingRate D3D12Utility::ConvertPixelShadingRate(D3D12_SHADING_RATE value)
{
    return D3D12Utility::GetVariableShadingRateMapping(value).variableShadingRate;
}


D3D12_SHADING_RATE_COMBINER D3D12Utility::ConvertShadingRateCombiner(rhi::ShadingRateCombiner value)
{
    return D3D12Utility::GetShadingRateCombinerMapping(value).d3d12ShadingRateCombiner;

}

rhi::ShadingRateCombiner D3D12Utility::ConvertShadingRateCombiner(D3D12_SHADING_RATE_COMBINER value)
{
    return D3D12Utility::GetShadingRateCombinerMapping(value).shadingRateCombiner;
}


void D3D12Utility::TranslateBlendState(const rhi::BlendState& inState, D3D12_BLEND_DESC& outState)
{
    outState.AlphaToCoverageEnable = inState.alphaToCoverageEnable;
    outState.IndependentBlendEnable = true;

    for (uint32_t i = 0; i < MAX_RENDER_TARGETS; i++)
    {
        const rhi::BlendState::RenderTarget& src = inState.targets[i];
        D3D12_RENDER_TARGET_BLEND_DESC& dst = outState.RenderTarget[i];

        dst.BlendEnable = src.blendEnable ? TRUE : FALSE;
        dst.SrcBlend = ConvertBlendValue(src.srcBlend);
        dst.DestBlend = ConvertBlendValue(src.destBlend);
        dst.BlendOp = ConvertBlendOp(src.blendOp);
        dst.SrcBlendAlpha = ConvertBlendValue(src.srcBlendAlpha);
        dst.DestBlendAlpha = ConvertBlendValue(src.destBlendAlpha);
        dst.BlendOpAlpha = ConvertBlendOp(src.blendOpAlpha);
        if (src.colorWriteMask.Test(ColorMask::Red))
        {
            dst.RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_RED;
        }
        if (src.colorWriteMask.Test(ColorMask::Green))
        {
            dst.RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_GREEN;
        }
        if (src.colorWriteMask.Test(ColorMask::Blue))
        {
            dst.RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_BLUE;
        }
        if (src.colorWriteMask.Test(ColorMask::Alpha))
        {
            dst.RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALPHA;
        }
    }
}

void D3D12Utility::TranslateBlendState(const D3D12_BLEND_DESC& inState, rhi::BlendState& outState)
{
    outState.alphaToCoverageEnable = inState.AlphaToCoverageEnable;

    for (uint32_t i = 0; i < MAX_RENDER_TARGETS; i++)
    {
        const D3D12_RENDER_TARGET_BLEND_DESC& src = inState.RenderTarget[i];
        rhi::BlendState::RenderTarget& dst = outState.targets[i];

        dst.blendEnable = src.BlendEnable ? true : false;
        dst.srcBlend = ConvertBlendValue(src.SrcBlend);
        dst.destBlend = ConvertBlendValue(src.DestBlend);
        dst.blendOp = ConvertBlendOp(src.BlendOp);
        dst.srcBlendAlpha = ConvertBlendValue(src.SrcBlendAlpha);
        dst.destBlendAlpha = ConvertBlendValue(src.DestBlendAlpha);
        dst.blendOpAlpha = ConvertBlendOp(src.BlendOpAlpha);
        if (core::EnumBitSetUtility::Check(src.RenderTargetWriteMask, D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_RED))
        {
            dst.colorWriteMask.Set(ColorMask::Red);
        }
        if (core::EnumBitSetUtility::Check(src.RenderTargetWriteMask, D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_GREEN))
        {
            dst.colorWriteMask.Set(ColorMask::Green);
        }
        if (core::EnumBitSetUtility::Check(src.RenderTargetWriteMask, D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_BLUE))
        {
            dst.colorWriteMask.Set(ColorMask::Blue);
        }
        if (core::EnumBitSetUtility::Check(src.RenderTargetWriteMask, D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALPHA))
        {
            dst.colorWriteMask.Set(ColorMask::Alpha);
        }
    }
}

void D3D12Utility::TranslateDepthStencilState(const rhi::DepthStencilState& inState, D3D12_DEPTH_STENCIL_DESC& outState)
{
    outState.DepthEnable = inState.depthTestEnable ? TRUE : FALSE;
    outState.DepthWriteMask = inState.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    outState.DepthFunc = ConvertComparisonFunc(inState.depthFunc);
    outState.StencilEnable = inState.stencilEnable ? TRUE : FALSE;
    outState.StencilReadMask = (UINT8)inState.stencilReadMask;
    outState.StencilWriteMask = (UINT8)inState.stencilWriteMask;
    outState.FrontFace.StencilFailOp = ConvertStencilOp(inState.frontFaceStencil.failOp);
    outState.FrontFace.StencilDepthFailOp = ConvertStencilOp(inState.frontFaceStencil.depthFailOp);
    outState.FrontFace.StencilPassOp = ConvertStencilOp(inState.frontFaceStencil.passOp);
    outState.FrontFace.StencilFunc = ConvertComparisonFunc(inState.frontFaceStencil.stencilFunc);
    outState.BackFace.StencilFailOp = ConvertStencilOp(inState.backFaceStencil.failOp);
    outState.BackFace.StencilDepthFailOp = ConvertStencilOp(inState.backFaceStencil.depthFailOp);
    outState.BackFace.StencilPassOp = ConvertStencilOp(inState.backFaceStencil.passOp);
    outState.BackFace.StencilFunc = ConvertComparisonFunc(inState.backFaceStencil.stencilFunc);
}

void D3D12Utility::TranslateDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& inState, rhi::DepthStencilState& outState)
{
    outState.depthTestEnable = inState.DepthEnable ? true : false;
    outState.depthWriteEnable = inState.DepthWriteMask == D3D12_DEPTH_WRITE_MASK_ALL ? true : false;
    outState.depthFunc = ConvertComparisonFunc(inState.DepthFunc);
    outState.stencilEnable = inState.StencilEnable ? true : false;
    outState.stencilReadMask = (uint8_t)inState.StencilReadMask;
    outState.stencilWriteMask = (uint8_t)inState.StencilWriteMask;
    outState.frontFaceStencil.failOp = ConvertStencilOp(inState.FrontFace.StencilFailOp);
    outState.frontFaceStencil.depthFailOp = ConvertStencilOp(inState.FrontFace.StencilDepthFailOp);
    outState.frontFaceStencil.passOp = ConvertStencilOp(inState.FrontFace.StencilPassOp);
    outState.frontFaceStencil.stencilFunc = ConvertComparisonFunc(inState.FrontFace.StencilFunc);
    outState.backFaceStencil.failOp = ConvertStencilOp(inState.BackFace.StencilFailOp);
    outState.backFaceStencil.depthFailOp = ConvertStencilOp(inState.BackFace.StencilDepthFailOp);
    outState.backFaceStencil.passOp = ConvertStencilOp(inState.BackFace.StencilPassOp);
    outState.backFaceStencil.stencilFunc = ConvertComparisonFunc(inState.BackFace.StencilFunc);
}

void D3D12Utility::TranslateRasterizerState(const rhi::RasterState& inState, D3D12_RASTERIZER_DESC& outState)
{
    switch (inState.fillMode)
    {
    case RasterFillMode::Solid:
        outState.FillMode = D3D12_FILL_MODE_SOLID;
        break;
    case RasterFillMode::Wireframe:
        outState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        break;
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Invalid Enum RasterFillMode");
        break;
    }

    switch (inState.cullMode)
    {
    case RasterCullMode::Back:
        outState.CullMode = D3D12_CULL_MODE_BACK;
        break;
    case RasterCullMode::Front:
        outState.CullMode = D3D12_CULL_MODE_FRONT;
        break;
    case RasterCullMode::None:
        outState.CullMode = D3D12_CULL_MODE_NONE;
        break;
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Invalid Enum RasterCullMode");
        break;
    }

    outState.FrontCounterClockwise = inState.frontCounterClockwise ? TRUE : FALSE;
    outState.DepthBias = inState.depthBias;
    outState.DepthBiasClamp = inState.depthBiasClamp;
    outState.SlopeScaledDepthBias = inState.slopeScaledDepthBias;
    outState.DepthClipEnable = inState.depthClipEnable ? TRUE : FALSE;
    outState.MultisampleEnable = inState.multisampleEnable ? TRUE : FALSE;
    outState.AntialiasedLineEnable = inState.antialiasedLineEnable ? TRUE : FALSE;
    outState.ConservativeRaster = inState.conservativeRasterEnable ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    outState.ForcedSampleCount = inState.forcedSampleCount;
}

void D3D12Utility::TranslateRasterizerState(const D3D12_RASTERIZER_DESC& inState, rhi::RasterState& outState)
{
    switch (inState.FillMode)
    {
    case D3D12_FILL_MODE_SOLID:
        outState.fillMode = RasterFillMode::Solid;
        break;
    case D3D12_FILL_MODE_WIREFRAME:
        outState.fillMode = RasterFillMode::Wireframe;
        break;
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Invalid Enum RasterFillMode");
        break;
    }

    switch (inState.CullMode)
    {
    case D3D12_CULL_MODE_BACK:
        outState.cullMode = RasterCullMode::Back;
        break;
    case D3D12_CULL_MODE_FRONT:
        outState.cullMode = RasterCullMode::Front;
        break;
    case D3D12_CULL_MODE_NONE:
        outState.cullMode = RasterCullMode::None;
        break;
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Invalid Enum RasterCullMode");
        break;
    }

    outState.frontCounterClockwise = inState.FrontCounterClockwise ? true : false;
    outState.depthBias = inState.DepthBias;
    outState.depthBiasClamp = inState.DepthBiasClamp;
    outState.slopeScaledDepthBias = inState.SlopeScaledDepthBias;
    outState.depthClipEnable = inState.DepthClipEnable ? true : false;
    outState.multisampleEnable = inState.MultisampleEnable ? true : false;
    outState.antialiasedLineEnable = inState.AntialiasedLineEnable ? true : false;
    outState.conservativeRasterEnable = inState.ConservativeRaster == D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON ? true : false;
    outState.forcedSampleCount = static_cast<uint8_t>(inState.ForcedSampleCount);
}


D3D12_RESOURCE_DESC D3D12Utility::ConvertTextureDesc(const TextureDesc& d)
{
    const auto& formatMapping = D3D12Utility::GetDxgiFormatMapping(d.format);
    const FormatInfo& formatInfo = GetFormatInfo(d.format);

    D3D12_RESOURCE_DESC desc = {};
    desc.Width = d.width;
    desc.Height = d.height;
    desc.MipLevels = UINT16(d.mipLevels);
    desc.Format = d.typeless ? formatMapping.resourceFormat : formatMapping.rtvFormat;
    desc.SampleDesc.Count = d.sampleCount;
    desc.SampleDesc.Quality = d.sampleQuality;

    switch (d.dimension)
    {
    case TextureDimension::Texture1D:
    case TextureDimension::Texture1DArray:
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        desc.DepthOrArraySize = UINT16(d.arraySize);
        break;
    case TextureDimension::Texture2D:
    case TextureDimension::Texture2DArray:
    case TextureDimension::TextureCube:
    case TextureDimension::TextureCubeArray:
    case TextureDimension::Texture2DMS:
    case TextureDimension::Texture2DMSArray:
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.DepthOrArraySize = UINT16(d.arraySize);
        break;
    case TextureDimension::Texture3D:
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        desc.DepthOrArraySize = UINT16(d.depth);
        break;
    case TextureDimension::Unknown:
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Alert, "Invalid Enum TextureDimension");
        break;
    }

    if (d.renderTarget)
    {
        if (formatInfo.hasDepth || formatInfo.hasStencil)
        {
            desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        } else
        {
            desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }
    }

    if (d.uav)
    {
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    return desc;
}

D3D12_CLEAR_VALUE D3D12Utility::ConvertTextureClearValue(const TextureDesc& d)
{
    const auto& formatMapping = D3D12Utility::GetDxgiFormatMapping(d.format);
    const FormatInfo& formatInfo = GetFormatInfo(d.format);
    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = formatMapping.rtvFormat;
    if (formatInfo.hasDepth || formatInfo.hasStencil)
    {
        clearValue.DepthStencil.Depth = d.clearDepth;
        clearValue.DepthStencil.Stencil = d.clearStencil;
    } else
    {
        clearValue.Color[0] = d.clearColor.r;
        clearValue.Color[1] = d.clearColor.g;
        clearValue.Color[2] = d.clearColor.b;
        clearValue.Color[3] = d.clearColor.a;
    }

    return clearValue;
}

D3D12_RESOURCE_STATES D3D12Utility::ConvertResourceStates(ResourceStatesBitset stateBits)
{
    if (stateBits.Test(ResourceStates::Common))
    {
        return D3D12_RESOURCE_STATE_COMMON;
    }

    D3D12_RESOURCE_STATES result = D3D12_RESOURCE_STATE_COMMON;

    if (stateBits.Test(ResourceStates::ConstantBuffer))
    {
        result |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    }

    if (stateBits.Test(ResourceStates::VertexBuffer) != 0)
    {
        result |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    }

    if (stateBits.Test(ResourceStates::IndexBuffer) != 0)
    {
        result |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
    }

    if (stateBits.Test(ResourceStates::IndirectArgument) != 0)
    {
        result |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    }

    if (stateBits.Test(ResourceStates::ShaderResource))
    {
        result |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    }

    if (stateBits.Test(ResourceStates::UnorderedAccess))
    {
        result |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    }

    if (stateBits.Test(ResourceStates::RenderTarget))
    {
        result |= D3D12_RESOURCE_STATE_RENDER_TARGET;
    }

    if (stateBits.Test(ResourceStates::DepthWrite))
    {
        result |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }

    if (stateBits.Test(ResourceStates::DepthRead))
    {
        result |= D3D12_RESOURCE_STATE_DEPTH_READ;
    }

    if (stateBits.Test(ResourceStates::StreamOut))
    {
        result |= D3D12_RESOURCE_STATE_STREAM_OUT;
    }

    if (stateBits.Test(ResourceStates::CopyDest))
    {
        result |= D3D12_RESOURCE_STATE_COPY_DEST;
    }

    if (stateBits.Test(ResourceStates::CopySource))
    {
        result |= D3D12_RESOURCE_STATE_COPY_SOURCE;
    }
    if (stateBits.Test(ResourceStates::ResolveDest))
    {
        result |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
    }
    if (stateBits.Test(ResourceStates::ResolveSource))
    {
        result |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
    }

    if (stateBits.Test(ResourceStates::Present))
    {
        result |= D3D12_RESOURCE_STATE_PRESENT;
    }

    if (stateBits.Test(ResourceStates::AccelStructRead))
    {
        result |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    }

    if (stateBits.Test(ResourceStates::AccelStructWrite))
    {
        result |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    }

    if (stateBits.Test(ResourceStates::AccelStructBuildInput))
    {
        result |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    }

    if (stateBits.Test(ResourceStates::AccelStructBuildBlas))
    {
        result |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    }

    if (stateBits.Test(ResourceStates::ShadingRateSurface))
    {
        result |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
    }

    if (stateBits.Test(ResourceStates::OpacityMicromapBuildInput))
    {
        result |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    }

    if (stateBits.Test(ResourceStates::OpacityMicromapWrite))
    {
        result |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    }

    return result;
}

D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS D3D12Utility::ConvertAcceslerationStructureBuildFlags(const rt::AccelStructBuildFlagsBitset& buildFlags)
{
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS asFlag = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
    if (buildFlags.Test(rt::AccelStructBuildFlags::AllowUpdate))
    {
        asFlag |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
    }
    if (buildFlags.Test(rt::AccelStructBuildFlags::AllowCompaction))
    {
        asFlag |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
    }
    if (buildFlags.Test(rt::AccelStructBuildFlags::PreferFastTrace))
    {
        asFlag |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    }
    if (buildFlags.Test(rt::AccelStructBuildFlags::PreferFastBuild))
    {
        asFlag |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
    }
    if (buildFlags.Test(rt::AccelStructBuildFlags::MinimizeMemory))
    {
        asFlag |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY;
    }
    if (buildFlags.Test(rt::AccelStructBuildFlags::PerformUpdate))
    {
        asFlag |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
    }
    return asFlag;
}

DX12ViewportState D3D12Utility::ConvertViewportState(const RasterState& rasterState, const FrameBufferInfoEx& framebufferInfo, const ViewportState& vpState)
{
    DX12ViewportState ret;

    ret.numViewports = UINT(vpState.viewports.size());
    for (size_t rt = 0; rt < vpState.viewports.size(); rt++)
    {
        ret.viewports[rt].TopLeftX = vpState.viewports[rt].min_x;
        ret.viewports[rt].TopLeftY = vpState.viewports[rt].min_y;
        ret.viewports[rt].Width = vpState.viewports[rt].max_x - vpState.viewports[rt].min_x;
        ret.viewports[rt].Height = vpState.viewports[rt].max_y - vpState.viewports[rt].min_y;
        ret.viewports[rt].MinDepth = vpState.viewports[rt].min_z;
        ret.viewports[rt].MaxDepth = vpState.viewports[rt].max_z;
    }

    ret.numScissorRects = UINT(vpState.scissorRects.size());
    for (size_t rt = 0; rt < vpState.scissorRects.size(); rt++)
    {
        if (rasterState.scissorEnable)
        {
            ret.scissorRects[rt].left = (LONG)vpState.scissorRects[rt].min_x;
            ret.scissorRects[rt].top = (LONG)vpState.scissorRects[rt].min_y;
            ret.scissorRects[rt].right = (LONG)vpState.scissorRects[rt].max_x;
            ret.scissorRects[rt].bottom = (LONG)vpState.scissorRects[rt].max_y;
        } else
        {
            ret.scissorRects[rt].left = (LONG)vpState.viewports[rt].min_x;
            ret.scissorRects[rt].top = (LONG)vpState.viewports[rt].min_y;
            ret.scissorRects[rt].right = (LONG)vpState.viewports[rt].max_x;
            ret.scissorRects[rt].bottom = (LONG)vpState.viewports[rt].max_y;

            if (framebufferInfo.width > 0)
            {
                ret.scissorRects[rt].left = math::TMax(ret.scissorRects[rt].left, LONG(0));
                ret.scissorRects[rt].top = math::TMax(ret.scissorRects[rt].top, LONG(0));
                ret.scissorRects[rt].right = math::TMin(ret.scissorRects[rt].right, LONG(framebufferInfo.width));
                ret.scissorRects[rt].bottom = math::TMin(ret.scissorRects[rt].bottom, LONG(framebufferInfo.height));
            }
        }
    }

    return ret;
}

void D3D12Utility::WaitForFence(ID3D12Fence* fence, uint64_t value, HANDLE event) 
{
    if (fence->GetCompletedValue() < value)
    {
        ResetEvent(event);
        fence->SetEventOnCompletion(value, event);
        WaitForSingleObject(event, INFINITE);
    }
}

bool D3D12Utility::IsBlendFactorRequired(rhi::BlendFactor value) 
{ 
    return false; 
}

uint32_t D3D12Utility::CalcSubresource(uint32_t mipSlice, uint32_t arraySlice, uint32_t planeSlice, uint32_t mipLevels, uint32_t arraySize) 
{
    return mipSlice + (arraySlice * mipLevels) + (planeSlice * mipLevels * arraySize);
}

}
}
}
