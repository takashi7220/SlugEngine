#pragma once

#include "rhi/Rhi.hpp"

namespace slug
{
namespace render
{

using Color = rhi::Color;

enum class FormatType : uint8_t
{
    UNKNOWN = rhi::FormatType::UNKNOWN,

    R8_UINT = rhi::FormatType::R8_UINT,
    R8_SINT = rhi::FormatType::R8_SINT,
    R8_UNORM = rhi::FormatType::R8_UNORM,
    R8_SNORM = rhi::FormatType::R8_SNORM,
    RG8_UINT = rhi::FormatType::RG8_UINT,
    RG8_SINT = rhi::FormatType::RG8_SINT,
    RG8_UNORM = rhi::FormatType::RG8_UNORM,
    RG8_SNORM = rhi::FormatType::RG8_SNORM,
    R16_UINT = rhi::FormatType::R16_UINT,
    R16_SINT = rhi::FormatType::R16_SINT,
    R16_UNORM = rhi::FormatType::R16_UNORM,
    R16_SNORM = rhi::FormatType::R16_SNORM,
    R16_FLOAT = rhi::FormatType::R16_FLOAT,
    RGBA8_UINT = rhi::FormatType::RGBA8_UINT,
    RGBA8_SINT = rhi::FormatType::RGBA8_SINT,
    RGBA8_UNORM = rhi::FormatType::RGBA8_UNORM,
    RGBA8_SNORM = rhi::FormatType::RGBA8_SNORM,
    SRGBA8_UNORM = rhi::FormatType::SRGBA8_UNORM,
    SBGRA8_UNORM = rhi::FormatType::SBGRA8_UNORM,
    R10G10B10A2_UNORM = rhi::FormatType::R10G10B10A2_UNORM,
    R11G11B10_FLOAT = rhi::FormatType::R11G11B10_FLOAT,
    RG16_UINT = rhi::FormatType::RG16_UINT,
    RG16_SINT = rhi::FormatType::RG16_SINT,
    RG16_UNORM = rhi::FormatType::RG16_UNORM,
    RG16_SNORM = rhi::FormatType::RG16_SNORM,
    RG16_FLOAT = rhi::FormatType::RG16_FLOAT,
    R32_UINT = rhi::FormatType::R32_UINT,
    R32_SINT = rhi::FormatType::R32_SINT,
    R32_FLOAT = rhi::FormatType::R32_FLOAT,
    RGBA16_UINT = rhi::FormatType::RGBA16_UINT,
    RGBA16_SINT = rhi::FormatType::RGBA16_SINT,
    RGBA16_FLOAT = rhi::FormatType::RGBA16_FLOAT,
    RGBA16_UNORM = rhi::FormatType::RGBA16_UNORM,
    RGBA16_SNORM = rhi::FormatType::RGBA16_SNORM,
    RG32_UINT = rhi::FormatType::RG32_UINT,
    RG32_SINT = rhi::FormatType::RG32_SINT,
    RG32_FLOAT = rhi::FormatType::RG32_FLOAT,
    RGB32_UINT = rhi::FormatType::RGB32_UINT,
    RGB32_SINT = rhi::FormatType::RGB32_SINT,
    RGB32_FLOAT = rhi::FormatType::RGB32_FLOAT,
    RGBA32_UINT = rhi::FormatType::RGBA32_UINT,
    RGBA32_SINT = rhi::FormatType::RGBA32_SINT,
    RGBA32_FLOAT = rhi::FormatType::RGBA32_FLOAT,

    D16 = rhi::FormatType::D16,
    D24S8 = rhi::FormatType::D24S8,
    X24G8_UINT = rhi::FormatType::X24G8_UINT,
    D32 = rhi::FormatType::D32,
    D32S8 = rhi::FormatType::D32S8,
    X32G8_UINT = rhi::FormatType::X32G8_UINT,

    BC1_UNORM = rhi::FormatType::BC1_UNORM,
    BC1_UNORM_SRGB = rhi::FormatType::BC1_UNORM_SRGB,
    BC2_UNORM = rhi::FormatType::BC2_UNORM,
    BC2_UNORM_SRGB = rhi::FormatType::BC2_UNORM_SRGB,
    BC3_UNORM = rhi::FormatType::BC3_UNORM,
    BC3_UNORM_SRGB = rhi::FormatType::BC2_UNORM_SRGB,
    BC4_UNORM = rhi::FormatType::BC4_UNORM,
    BC4_SNORM = rhi::FormatType::BC4_SNORM,
    BC5_UNORM = rhi::FormatType::BC5_UNORM,
    BC5_SNORM = rhi::FormatType::BC5_SNORM,
    BC6H_UFLOAT = rhi::FormatType::BC6H_UFLOAT,
    BC6H_SFLOAT = rhi::FormatType::BC6H_SFLOAT,
    BC7_UNORM = rhi::FormatType::BC7_UNORM,
    BC7_UNORM_SRGB = rhi::FormatType::BC7_UNORM_SRGB,

    COUNT = rhi::FormatType::COUNT,
};

enum class TextureDimension : uint8_t
{
    Unknown = rhi::TextureDimension::Unknown,
    Texture1D = rhi::TextureDimension::Texture1D,
    Texture1DArray = rhi::TextureDimension::Texture1DArray,
    Texture2D = rhi::TextureDimension::Texture2D,
    Texture2DArray = rhi::TextureDimension::Texture2DArray,
    TextureCube = rhi::TextureDimension::TextureCube,
    TextureCubeArray = rhi::TextureDimension::TextureCubeArray,
    Texture2DMS = rhi::TextureDimension::Texture2DMS,
    Texture2DMSArray = rhi::TextureDimension::Texture2DMSArray,
    Texture3D = rhi::TextureDimension::Texture3D
};

enum class SamplerAddressMode : uint8_t
{
    Clamp = rhi::SamplerAddressMode::Clamp,
    Wrap = rhi::SamplerAddressMode::Wrap,
    Border = rhi::SamplerAddressMode::Border,
    Mirror = rhi::SamplerAddressMode::Mirror
};

enum class SamplerReductionType : uint8_t
{
    Standard = rhi::SamplerReductionType::Standard,
    Comparison = rhi::SamplerReductionType::Comparison,
    Minimum = rhi::SamplerReductionType::Minimum,
    Maximum = rhi::SamplerReductionType::Maximum
};

enum class ComparisonFunc : uint8_t
{
    Never = rhi::ComparisonFunc::Never,
    Less = rhi::ComparisonFunc::Less,
    Equal = rhi::ComparisonFunc::Equal,
    LessOrEqual = rhi::ComparisonFunc::LessOrEqual,
    Greater = rhi::ComparisonFunc::Greater,
    NotEqual = rhi::ComparisonFunc::NotEqual,
    GreaterOrEqual = rhi::ComparisonFunc::GreaterOrEqual,
    Always = rhi::ComparisonFunc::Always
};

enum class StencilOp : uint8_t
{
    Keep = rhi::StencilOp::Keep,
    Zero = rhi::StencilOp::Zero,
    Replace = rhi::StencilOp::Replace,
    IncrementAndClamp = rhi::StencilOp::IncrementAndClamp,
    DecrementAndClamp = rhi::StencilOp::DecrementAndClamp,
    Invert = rhi::StencilOp::Invert,
    IncrementAndWrap = rhi::StencilOp::IncrementAndWrap,
    DecrementAndWrap = rhi::StencilOp::DecrementAndWrap
};

enum class RasterCullMode
{
    Back = rhi::RasterCullMode::Back,
    Front = rhi::RasterCullMode::Front,
    None = rhi::RasterCullMode::None,
    FromObject,
};

enum class GraphicsApiType
{
    D3D12,
    D3D11,
    Vulkan
};

enum class ShaderType
{
    Vertex = rhi::ShaderType::Vertex,
    Pixel = rhi::ShaderType::Pixel,
    Geometry = rhi::ShaderType::Geometry,
    Hull = rhi::ShaderType::Hull,
    Domain = rhi::ShaderType::Domain,
    Compute = rhi::ShaderType::Compute,
};

}
}
