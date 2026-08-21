#pragma once

#include "rhi/Rhi.hpp"

namespace slug::render_core
{

using Color = rhi::Color;

enum class FormatType : uint8_t
{
    UNKNOWN = static_cast<uint8_t>(rhi::FormatType::UNKNOWN),

    R8_UINT = static_cast<uint8_t>(rhi::FormatType::R8_UINT),
    R8_SINT = static_cast<uint8_t>(rhi::FormatType::R8_SINT),
    R8_UNORM = static_cast<uint8_t>(rhi::FormatType::R8_UNORM),
    R8_SNORM = static_cast<uint8_t>(rhi::FormatType::R8_SNORM),
    RG8_UINT = static_cast<uint8_t>(rhi::FormatType::RG8_UINT),
    RG8_SINT = static_cast<uint8_t>(rhi::FormatType::RG8_SINT),
    RG8_UNORM = static_cast<uint8_t>(rhi::FormatType::RG8_UNORM),
    RG8_SNORM = static_cast<uint8_t>(rhi::FormatType::RG8_SNORM),
    R16_UINT = static_cast<uint8_t>(rhi::FormatType::R16_UINT),
    R16_SINT = static_cast<uint8_t>(rhi::FormatType::R16_SINT),
    R16_UNORM = static_cast<uint8_t>(rhi::FormatType::R16_UNORM),
    R16_SNORM = static_cast<uint8_t>(rhi::FormatType::R16_SNORM),
    R16_FLOAT = static_cast<uint8_t>(rhi::FormatType::R16_FLOAT),
    RGBA8_UINT = static_cast<uint8_t>(rhi::FormatType::RGBA8_UINT),
    RGBA8_SINT = static_cast<uint8_t>(rhi::FormatType::RGBA8_SINT),
    RGBA8_UNORM = static_cast<uint8_t>(rhi::FormatType::RGBA8_UNORM),
    RGBA8_SNORM = static_cast<uint8_t>(rhi::FormatType::RGBA8_SNORM),
    SRGBA8_UNORM = static_cast<uint8_t>(rhi::FormatType::SRGBA8_UNORM),
    SBGRA8_UNORM = static_cast<uint8_t>(rhi::FormatType::SBGRA8_UNORM),
    R10G10B10A2_UNORM = static_cast<uint8_t>(rhi::FormatType::R10G10B10A2_UNORM),
    R11G11B10_FLOAT = static_cast<uint8_t>(rhi::FormatType::R11G11B10_FLOAT),
    RG16_UINT = static_cast<uint8_t>(rhi::FormatType::RG16_UINT),
    RG16_SINT = static_cast<uint8_t>(rhi::FormatType::RG16_SINT),
    RG16_UNORM = static_cast<uint8_t>(rhi::FormatType::RG16_UNORM),
    RG16_SNORM = static_cast<uint8_t>(rhi::FormatType::RG16_SNORM),
    RG16_FLOAT = static_cast<uint8_t>(rhi::FormatType::RG16_FLOAT),
    R32_UINT = static_cast<uint8_t>(rhi::FormatType::R32_UINT),
    R32_SINT = static_cast<uint8_t>(rhi::FormatType::R32_SINT),
    R32_FLOAT = static_cast<uint8_t>(rhi::FormatType::R32_FLOAT),
    RGBA16_UINT = static_cast<uint8_t>(rhi::FormatType::RGBA16_UINT),
    RGBA16_SINT = static_cast<uint8_t>(rhi::FormatType::RGBA16_SINT),
    RGBA16_FLOAT = static_cast<uint8_t>(rhi::FormatType::RGBA16_FLOAT),
    RGBA16_UNORM = static_cast<uint8_t>(rhi::FormatType::RGBA16_UNORM),
    RGBA16_SNORM = static_cast<uint8_t>(rhi::FormatType::RGBA16_SNORM),
    RG32_UINT = static_cast<uint8_t>(rhi::FormatType::RG32_UINT),
    RG32_SINT = static_cast<uint8_t>(rhi::FormatType::RG32_SINT),
    RG32_FLOAT = static_cast<uint8_t>(rhi::FormatType::RG32_FLOAT),
    RGB32_UINT = static_cast<uint8_t>(rhi::FormatType::RGB32_UINT),
    RGB32_SINT = static_cast<uint8_t>(rhi::FormatType::RGB32_SINT),
    RGB32_FLOAT = static_cast<uint8_t>(rhi::FormatType::RGB32_FLOAT),
    RGBA32_UINT = static_cast<uint8_t>(rhi::FormatType::RGBA32_UINT),
    RGBA32_SINT = static_cast<uint8_t>(rhi::FormatType::RGBA32_SINT),
    RGBA32_FLOAT = static_cast<uint8_t>(rhi::FormatType::RGBA32_FLOAT),

    D16 = static_cast<uint8_t>(rhi::FormatType::D16),
    D24S8 = static_cast<uint8_t>(rhi::FormatType::D24S8),
    X24G8_UINT = static_cast<uint8_t>(rhi::FormatType::X24G8_UINT),
    D32 = static_cast<uint8_t>(rhi::FormatType::D32),
    D32S8 = static_cast<uint8_t>(rhi::FormatType::D32S8),
    X32G8_UINT = static_cast<uint8_t>(rhi::FormatType::X32G8_UINT),

    BC1_UNORM = static_cast<uint8_t>(rhi::FormatType::BC1_UNORM),
    BC1_UNORM_SRGB = static_cast<uint8_t>(rhi::FormatType::BC1_UNORM_SRGB),
    BC2_UNORM = static_cast<uint8_t>(rhi::FormatType::BC2_UNORM),
    BC2_UNORM_SRGB = static_cast<uint8_t>(rhi::FormatType::BC2_UNORM_SRGB),
    BC3_UNORM = static_cast<uint8_t>(rhi::FormatType::BC3_UNORM),
    BC3_UNORM_SRGB = static_cast<uint8_t>(rhi::FormatType::BC2_UNORM_SRGB),
    BC4_UNORM = static_cast<uint8_t>(rhi::FormatType::BC4_UNORM),
    BC4_SNORM = static_cast<uint8_t>(rhi::FormatType::BC4_SNORM),
    BC5_UNORM = static_cast<uint8_t>(rhi::FormatType::BC5_UNORM),
    BC5_SNORM = static_cast<uint8_t>(rhi::FormatType::BC5_SNORM),
    BC6H_UFLOAT = static_cast<uint8_t>(rhi::FormatType::BC6H_UFLOAT),
    BC6H_SFLOAT = static_cast<uint8_t>(rhi::FormatType::BC6H_SFLOAT),
    BC7_UNORM = static_cast<uint8_t>(rhi::FormatType::BC7_UNORM),
    BC7_UNORM_SRGB = static_cast<uint8_t>(rhi::FormatType::BC7_UNORM_SRGB),

    COUNT = static_cast<uint8_t>(rhi::FormatType::COUNT),
};

enum class TextureDimension : uint8_t
{
    Unknown = static_cast<uint8_t>(rhi::TextureDimension::Unknown),
    Texture1D = static_cast<uint8_t>(rhi::TextureDimension::Texture1D),
    Texture1DArray = static_cast<uint8_t>(rhi::TextureDimension::Texture1DArray),
    Texture2D = static_cast<uint8_t>(rhi::TextureDimension::Texture2D),
    Texture2DArray = static_cast<uint8_t>(rhi::TextureDimension::Texture2DArray),
    TextureCube = static_cast<uint8_t>(rhi::TextureDimension::TextureCube),
    TextureCubeArray = static_cast<uint8_t>(rhi::TextureDimension::TextureCubeArray),
    Texture2DMS = static_cast<uint8_t>(rhi::TextureDimension::Texture2DMS),
    Texture2DMSArray = static_cast<uint8_t>(rhi::TextureDimension::Texture2DMSArray),
    Texture3D = static_cast<uint8_t>(rhi::TextureDimension::Texture3D)
};

enum class SamplerAddressMode : uint8_t
{
    Clamp = static_cast<uint8_t>(rhi::SamplerAddressMode::Clamp),
    Wrap = static_cast<uint8_t>(rhi::SamplerAddressMode::Wrap),
    Border = static_cast<uint8_t>(rhi::SamplerAddressMode::Border),
    Mirror = static_cast<uint8_t>(rhi::SamplerAddressMode::Mirror)
};

enum class SamplerReductionType : uint8_t
{
    Standard = static_cast<uint8_t>(rhi::SamplerReductionType::Standard),
    Comparison = static_cast<uint8_t>(rhi::SamplerReductionType::Comparison),
    Minimum = static_cast<uint8_t>(rhi::SamplerReductionType::Minimum),
    Maximum = static_cast<uint8_t>(rhi::SamplerReductionType::Maximum)
};

enum class ComparisonFunc : uint8_t
{
    Never = static_cast<uint8_t>(rhi::ComparisonFunc::Never),
    Less = static_cast<uint8_t>(rhi::ComparisonFunc::Less),
    Equal = static_cast<uint8_t>(rhi::ComparisonFunc::Equal),
    LessOrEqual = static_cast<uint8_t>(rhi::ComparisonFunc::LessOrEqual),
    Greater = static_cast<uint8_t>(rhi::ComparisonFunc::Greater),
    NotEqual = static_cast<uint8_t>(rhi::ComparisonFunc::NotEqual),
    GreaterOrEqual = static_cast<uint8_t>(rhi::ComparisonFunc::GreaterOrEqual),
    Always = static_cast<uint8_t>(rhi::ComparisonFunc::Always)
};

enum class StencilOp : uint8_t
{
    Keep = static_cast<uint8_t>(rhi::StencilOp::Keep),
    Zero = static_cast<uint8_t>(rhi::StencilOp::Zero),
    Replace = static_cast<uint8_t>(rhi::StencilOp::Replace),
    IncrementAndClamp = static_cast<uint8_t>(rhi::StencilOp::IncrementAndClamp),
    DecrementAndClamp = static_cast<uint8_t>(rhi::StencilOp::DecrementAndClamp),
    Invert = static_cast<uint8_t>(rhi::StencilOp::Invert),
    IncrementAndWrap = static_cast<uint8_t>(rhi::StencilOp::IncrementAndWrap),
    DecrementAndWrap = static_cast<uint8_t>(rhi::StencilOp::DecrementAndWrap)
};

enum class RasterCullMode
{
    Back = static_cast<uint8_t>(rhi::RasterCullMode::Back),
    Front = static_cast<uint8_t>(rhi::RasterCullMode::Front),
    None = static_cast<uint8_t>(rhi::RasterCullMode::None),
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
    Vertex = static_cast<uint8_t>(rhi::ShaderType::Vertex),
    Pixel = static_cast<uint8_t>(rhi::ShaderType::Pixel),
    Geometry = static_cast<uint8_t>(rhi::ShaderType::Geometry),
    Hull = static_cast<uint8_t>(rhi::ShaderType::Hull),
    Domain = static_cast<uint8_t>(rhi::ShaderType::Domain),
    Compute = static_cast<uint8_t>(rhi::ShaderType::Compute),
};

}
