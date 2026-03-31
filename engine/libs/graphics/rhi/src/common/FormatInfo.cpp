#include "rhi/Rhi.hpp"

namespace slug 
{
namespace rhi 
{
static const FormatInfo c_format_info[] = {
        { FormatType::UNKNOWN,           "UNKNOWN",           0,   0, FormatKind::Integer,      false, false, false, false, false, false, false, false },
        { FormatType::R8_UINT,           "R8_UINT",           1,   1, FormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { FormatType::R8_SINT,           "R8_SINT",           1,   1, FormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { FormatType::R8_UNORM,          "R8_UNORM",          1,   1, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { FormatType::R8_SNORM,          "R8_SNORM",          1,   1, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { FormatType::RG8_UINT,          "RG8_UINT",          2,   1, FormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { FormatType::RG8_SINT,          "RG8_SINT",          2,   1, FormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { FormatType::RG8_UNORM,         "RG8_UNORM",         2,   1, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { FormatType::RG8_SNORM,         "RG8_SNORM",         2,   1, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { FormatType::R16_UINT,          "R16_UINT",          2,   1, FormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { FormatType::R16_SINT,          "R16_SINT",          2,   1, FormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { FormatType::R16_UNORM,         "R16_UNORM",         2,   1, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { FormatType::R16_SNORM,         "R16_SNORM",         2,   1, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { FormatType::R16_FLOAT,         "R16_FLOAT",         2,   1, FormatKind::Float,        true,  false, false, false, false, false, true,  false },
        { FormatType::BGRA4_UNORM,       "BGRA4_UNORM",       2,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::B5G6R5_UNORM,      "B5G6R5_UNORM",      2,   1, FormatKind::Normalized,   true,  true,  true,  false, false, false, false, false },
        { FormatType::B5G5R5A1_UNORM,    "B5G5R5A1_UNORM",    2,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::RGBA8_UINT,        "RGBA8_UINT",        4,   1, FormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { FormatType::RGBA8_SINT,        "RGBA8_SINT",        4,   1, FormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { FormatType::RGBA8_UNORM,       "RGBA8_UNORM",       4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::RGBA8_SNORM,       "RGBA8_SNORM",       4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::BGRA8_UNORM,       "BGRA8_UNORM",       4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::SRGBA8_UNORM,      "SRGBA8_UNORM",      4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { FormatType::SBGRA8_UNORM,      "SBGRA8_UNORM",      4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::R10G10B10A2_UNORM, "R10G10B10A2_UNORM", 4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::R11G11B10_FLOAT,   "R11G11B10_FLOAT",   4,   1, FormatKind::Float,        true,  true,  true,  false, false, false, false, false },
        { FormatType::RG16_UINT,         "RG16_UINT",         4,   1, FormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { FormatType::RG16_SINT,         "RG16_SINT",         4,   1, FormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { FormatType::RG16_UNORM,        "RG16_UNORM",        4,   1, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { FormatType::RG16_SNORM,        "RG16_SNORM",        4,   1, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { FormatType::RG16_FLOAT,        "RG16_FLOAT",        4,   1, FormatKind::Float,        true,  true,  false, false, false, false, true,  false },
        { FormatType::R32_UINT,          "R32_UINT",          4,   1, FormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { FormatType::R32_SINT,          "R32_SINT",          4,   1, FormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { FormatType::R32_FLOAT,         "R32_FLOAT",         4,   1, FormatKind::Float,        true,  false, false, false, false, false, true,  false },
        { FormatType::RGBA16_UINT,       "RGBA16_UINT",       8,   1, FormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { FormatType::RGBA16_SINT,       "RGBA16_SINT",       8,   1, FormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { FormatType::RGBA16_FLOAT,      "RGBA16_FLOAT",      8,   1, FormatKind::Float,        true,  true,  true,  true,  false, false, true,  false },
        { FormatType::RGBA16_UNORM,      "RGBA16_UNORM",      8,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::RGBA16_SNORM,      "RGBA16_SNORM",      8,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::RG32_UINT,         "RG32_UINT",         8,   1, FormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { FormatType::RG32_SINT,         "RG32_SINT",         8,   1, FormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { FormatType::RG32_FLOAT,        "RG32_FLOAT",        8,   1, FormatKind::Float,        true,  true,  false, false, false, false, true,  false },
        { FormatType::RGB32_UINT,        "RGB32_UINT",        12,  1, FormatKind::Integer,      true,  true,  true,  false, false, false, false, false },
        { FormatType::RGB32_SINT,        "RGB32_SINT",        12,  1, FormatKind::Integer,      true,  true,  true,  false, false, false, true,  false },
        { FormatType::RGB32_FLOAT,       "RGB32_FLOAT",       12,  1, FormatKind::Float,        true,  true,  true,  false, false, false, true,  false },
        { FormatType::RGBA32_UINT,       "RGBA32_UINT",       16,  1, FormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { FormatType::RGBA32_SINT,       "RGBA32_SINT",       16,  1, FormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { FormatType::RGBA32_FLOAT,      "RGBA32_FLOAT",      16,  1, FormatKind::Float,        true,  true,  true,  true,  false, false, true,  false },
        { FormatType::D16,               "D16",               2,   1, FormatKind::DepthStencil, false, false, false, false, true,  false, false, false },
        { FormatType::D24S8,             "D24S8",             4,   1, FormatKind::DepthStencil, false, false, false, false, true,  true,  false, false },
        { FormatType::X24G8_UINT,        "X24G8_UINT",        4,   1, FormatKind::Integer,      false, false, false, false, false, true,  false, false },
        { FormatType::D32,               "D32",               4,   1, FormatKind::DepthStencil, false, false, false, false, true,  false, false, false },
        { FormatType::D32S8,             "D32S8",             8,   1, FormatKind::DepthStencil, false, false, false, false, true,  true,  false, false },
        { FormatType::X32G8_UINT,        "X32G8_UINT",        8,   1, FormatKind::Integer,      false, false, false, false, false, true,  false, false },
        { FormatType::BC1_UNORM,         "BC1_UNORM",         8,   4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::BC1_UNORM_SRGB,    "BC1_UNORM_SRGB",    8,   4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { FormatType::BC2_UNORM,         "BC2_UNORM",         16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::BC2_UNORM_SRGB,    "BC2_UNORM_SRGB",    16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { FormatType::BC3_UNORM,         "BC3_UNORM",         16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::BC3_UNORM_SRGB,    "BC3_UNORM_SRGB",    16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { FormatType::BC4_UNORM,         "BC4_UNORM",         8,   4, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { FormatType::BC4_SNORM,         "BC4_SNORM",         8,   4, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { FormatType::BC5_UNORM,         "BC5_UNORM",         16,  4, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { FormatType::BC5_SNORM,         "BC5_SNORM",         16,  4, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { FormatType::BC6H_UFLOAT,       "BC6H_UFLOAT",       16,  4, FormatKind::Float,        true,  true,  true,  false, false, false, false, false },
        { FormatType::BC6H_SFLOAT,       "BC6H_SFLOAT",       16,  4, FormatKind::Float,        true,  true,  true,  false, false, false, true,  false },
        { FormatType::BC7_UNORM,         "BC7_UNORM",         16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { FormatType::BC7_UNORM_SRGB,    "BC7_UNORM_SRGB",    16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
    };

const FormatInfo& GetFormatInfo(FormatType format)
{
    SLUG_STATIC_ASSERT_MSG(sizeof(c_format_info) / sizeof(FormatInfo) == size_t(FormatType::COUNT),
        "The format info table doesn't have the right number of elements");

    if (uint32_t(format) >= uint32_t(FormatType::COUNT)) 
    {
        return c_format_info[0];
    }

    const FormatInfo& info = c_format_info[uint32_t(format)];
    SLUG_ASSERT(info.format == format);
    return info;
}
}
}