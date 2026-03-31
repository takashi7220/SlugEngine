#include "image/ImageUtility.hpp"
#include "image/DDS.hpp"
#include <algorithm>

#define SLUG_D3D11_RESOURCE_MISC_TEXTURECUBE 0x4

namespace slug
{
namespace image
{

struct ImageFormatMapping
{
    ImageFormat imageFormat;
    DXGI_FORMAT dxgiFormat;
    uint32_t bitsPerPixel;
};

const ImageFormatMapping IMAGE_FORMA_MAPPING[] =
{
    { ImageFormat::FORMAT_UNKNOWN,                  DXGI_FORMAT::DXGI_FORMAT_UNKNOWN,                   0 },
    { ImageFormat::FORMAT_R8_UINT,                  DXGI_FORMAT::DXGI_FORMAT_R8_UINT,                   8 },
    { ImageFormat::FORMAT_R8_SINT,                  DXGI_FORMAT::DXGI_FORMAT_R8_SINT,                   8 },
    { ImageFormat::FORMAT_R8_UNORM,                 DXGI_FORMAT::DXGI_FORMAT_R8_UNORM,                  8 },
    { ImageFormat::FORMAT_R8_SNORM,                 DXGI_FORMAT::DXGI_FORMAT_R8_SNORM,                  8 },
    { ImageFormat::FORMAT_R8G8_UINT,                DXGI_FORMAT::DXGI_FORMAT_R8G8_UINT,                 16 },
    { ImageFormat::FORMAT_R8G8_SINT,                DXGI_FORMAT::DXGI_FORMAT_R8G8_SINT,                 16 },
    { ImageFormat::FORMAT_R8G8_UNORM,               DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM,                16 },
    { ImageFormat::FORMAT_R8G8_SNORM,               DXGI_FORMAT::DXGI_FORMAT_R8G8_SNORM,                16 },
    { ImageFormat::FORMAT_R16_UINT,                 DXGI_FORMAT::DXGI_FORMAT_R16_UINT,                  16 },
    { ImageFormat::FORMAT_R16_SINT,                 DXGI_FORMAT::DXGI_FORMAT_R16_SINT,                  16 },
    { ImageFormat::FORMAT_R16_UNORM,                DXGI_FORMAT::DXGI_FORMAT_R16_UNORM,                 16 },
    { ImageFormat::FORMAT_R16_SNORM,                DXGI_FORMAT::DXGI_FORMAT_R16_SNORM,                 16 },
    { ImageFormat::FORMAT_R16_FLOAT,                DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT,                 16 },
    { ImageFormat::FORMAT_B4G4R4A4_UNORM,           DXGI_FORMAT::DXGI_FORMAT_B4G4R4A4_UNORM,            16 },
    { ImageFormat::FORMAT_B5G6R5_UNORM,             DXGI_FORMAT::DXGI_FORMAT_B5G6R5_UNORM,              16 },
    { ImageFormat::FORMAT_B5G5R5A1_UNORM,           DXGI_FORMAT::DXGI_FORMAT_B5G5R5A1_UNORM,            16 },
    { ImageFormat::FORMAT_R8G8B8A8_UINT,            DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT,             32 },
    { ImageFormat::FORMAT_R8G8B8A8_SINT,            DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SINT,             32 },
    { ImageFormat::FORMAT_R8G8B8A8_UNORM,           DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,            32 },
    { ImageFormat::FORMAT_R8G8B8A8_SNORM,           DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM,            32 },
    { ImageFormat::FORMAT_B8G8R8A8_UNORM,           DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,            32 },
    { ImageFormat::FORMAT_R8G8B8A8_UNORM_SRGB,      DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,       32 },
    { ImageFormat::FORMAT_B8G8R8A8_UNORM_SRGB,      DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,       32 },
    { ImageFormat::FORMAT_R10G10B10A2_UNORM,        DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UNORM,         32 },
    { ImageFormat::FORMAT_R11G11B10_FLOAT,          DXGI_FORMAT::DXGI_FORMAT_R11G11B10_FLOAT,           32 },
    { ImageFormat::FORMAT_R16G16_UINT,              DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT,               32 },
    { ImageFormat::FORMAT_R16G16_SINT,              DXGI_FORMAT::DXGI_FORMAT_R16G16_SINT,               32 },
    { ImageFormat::FORMAT_R16G16_UNORM,             DXGI_FORMAT::DXGI_FORMAT_R16G16_UNORM,              32 },
    { ImageFormat::FORMAT_R16G16_SNORM,             DXGI_FORMAT::DXGI_FORMAT_R16G16_SNORM,              32 },
    { ImageFormat::FORMAT_R16G16_FLOAT,             DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT,              32 },
    { ImageFormat::FORMAT_R32_UINT,                 DXGI_FORMAT::DXGI_FORMAT_R32_UINT,                  32 },
    { ImageFormat::FORMAT_R32_SINT,                 DXGI_FORMAT::DXGI_FORMAT_R32_SINT,                  32 },
    { ImageFormat::FORMAT_R32_FLOAT,                DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT,                 32 },
    { ImageFormat::FORMAT_R16G16B16A16_UINT,        DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UINT,         64 },
    { ImageFormat::FORMAT_R16G16B16A16_SINT,        DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SINT,         64 },
    { ImageFormat::FORMAT_R16G16B16A16_FLOAT,       DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT,        64 },
    { ImageFormat::FORMAT_R16G16B16A16_UNORM,       DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UNORM,        64 },
    { ImageFormat::FORMAT_R16G16B16A16_SNORM,       DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SNORM,        64 },
    { ImageFormat::FORMAT_R32G32_UINT,              DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT,               64 },
    { ImageFormat::FORMAT_R32G32_SINT,              DXGI_FORMAT::DXGI_FORMAT_R32G32_SINT,               64 },
    { ImageFormat::FORMAT_R32G32_FLOAT,             DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,              64 },
    { ImageFormat::FORMAT_R32G32B32_UINT,           DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT,            96 },
    { ImageFormat::FORMAT_R32G32B32_SINT,           DXGI_FORMAT::DXGI_FORMAT_R32G32B32_SINT,            96 },
    { ImageFormat::FORMAT_R32G32B32_FLOAT,          DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,           96 },
    { ImageFormat::FORMAT_R32G32B32A32_UINT,        DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT,         128 },
    { ImageFormat::FORMAT_R32G32B32A32_SINT,        DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT,         128 },
    { ImageFormat::FORMAT_R32G32B32A32_FLOAT,       DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,        128 },
    { ImageFormat::FORMAT_R16_UNORM,                DXGI_FORMAT::DXGI_FORMAT_R16_UNORM,                 16 },
    { ImageFormat::FORMAT_R24_UNORM_X8_TYPELESS,    DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS,     32 },
    { ImageFormat::FORMAT_X24_TYPELESS_G8_UINT,     DXGI_FORMAT::DXGI_FORMAT_X24_TYPELESS_G8_UINT,      32 },
    { ImageFormat::FORMAT_R32_FLOAT,                DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT,                 32 },
    { ImageFormat::FORMAT_R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,  64 },
    { ImageFormat::FORMAT_X32_TYPELESS_G8X24_UINT,  DXGI_FORMAT::DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,   64 },
    { ImageFormat::FORMAT_BC1_UNORM,                DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM,                 4 },
    { ImageFormat::FORMAT_BC1_UNORM_SRGB,           DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM_SRGB,            4 },
    { ImageFormat::FORMAT_BC2_UNORM,                DXGI_FORMAT::DXGI_FORMAT_BC2_UNORM,                 8 },
    { ImageFormat::FORMAT_BC2_UNORM_SRGB,           DXGI_FORMAT::DXGI_FORMAT_BC2_UNORM_SRGB,            8 },
    { ImageFormat::FORMAT_BC3_UNORM,                DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM,                 8 },
    { ImageFormat::FORMAT_BC3_UNORM_SRGB,           DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM_SRGB,            8 },
    { ImageFormat::FORMAT_BC4_UNORM,                DXGI_FORMAT::DXGI_FORMAT_BC4_UNORM,                 4 },
    { ImageFormat::FORMAT_BC4_SNORM,                DXGI_FORMAT::DXGI_FORMAT_BC4_SNORM,                 4 },
    { ImageFormat::FORMAT_BC5_UNORM,                DXGI_FORMAT::DXGI_FORMAT_BC5_UNORM,                 8 },
    { ImageFormat::FORMAT_BC5_SNORM,                DXGI_FORMAT::DXGI_FORMAT_BC5_SNORM,                 8 },
    { ImageFormat::FORMAT_BC6H_UF16,                DXGI_FORMAT::DXGI_FORMAT_BC6H_UF16,                 8 },
    { ImageFormat::FORMAT_BC6H_SF16,                DXGI_FORMAT::DXGI_FORMAT_BC6H_SF16,                 8 },
    { ImageFormat::FORMAT_BC7_UNORM,                DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM,                 8 },
    { ImageFormat::FORMAT_BC7_UNORM_SRGB,           DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM_SRGB,            8 },
};

#define SLUG_ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )


ImageFormat ConvertImageFormat(const DDS_PIXELFORMAT& ddpf, bool forceSRGB)
{
    if (ddpf.flags & DDS_RGB)
    {

        switch (ddpf.RGBBitCount)
        {
        case 32:
            if (SLUG_ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
            {
                return !forceSRGB ? ImageFormat::FORMAT_R8G8B8A8_UNORM : ImageFormat::FORMAT_R8G8B8A8_UNORM_SRGB;
            }

            if (SLUG_ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
            {
                return !forceSRGB ? ImageFormat::FORMAT_B8G8R8A8_UNORM : ImageFormat::FORMAT_B8G8R8A8_UNORM_SRGB;
            }

            if (SLUG_ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
            {
                return !forceSRGB ? ImageFormat::FORMAT_B8G8R8A8_UNORM : ImageFormat::FORMAT_B8G8R8A8_UNORM_SRGB;
            }

            if (SLUG_ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
            {
                return ImageFormat::FORMAT_R10G10B10A2_UNORM;
            }


            if (SLUG_ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
            {
                return ImageFormat::FORMAT_R16G16_UNORM;
            }

            if (SLUG_ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
            {
                return ImageFormat::FORMAT_R32_FLOAT;
            }
            break;

        case 24:
            break;

        case 16:
            if (SLUG_ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
            {
                return ImageFormat::FORMAT_B5G5R5A1_UNORM;
            }
            if (SLUG_ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000))
            {
                return ImageFormat::FORMAT_B5G6R5_UNORM;
            }

            if (SLUG_ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
            {
                return ImageFormat::FORMAT_B4G4R4A4_UNORM;
            }

            break;
        }
    } else if (ddpf.flags & DDS_LUMINANCE)
    {
        if (8 == ddpf.RGBBitCount)
        {
            if (SLUG_ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
            {
                return ImageFormat::FORMAT_R8_UNORM;
            }

            // No DXGI format maps to SLUG_ISBITMASK(0x0f,0x00,0x00,0xf0) aka D3DFMT_A4L4

            if (SLUG_ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
            {
                return ImageFormat::FORMAT_R8G8_UNORM;
            }
        }

        if (16 == ddpf.RGBBitCount)
        {
            if (SLUG_ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
            {
                return ImageFormat::FORMAT_R16_UNORM;
            }
            if (SLUG_ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
            {
                return ImageFormat::FORMAT_R8G8_UNORM;
            }
        }
    } else if (ddpf.flags & DDS_ALPHA)
    {
        if (8 == ddpf.RGBBitCount)
        {
            return ImageFormat::FORMAT_R8_UNORM;
        }
    } else if (ddpf.flags & DDS_BUMPDUDV)
    {
        if (16 == ddpf.RGBBitCount)
        {
            if (SLUG_ISBITMASK(0x00ff, 0xff00, 0x0000, 0x0000))
            {
                return ImageFormat::FORMAT_R8G8_SNORM;
            }
        }

        if (32 == ddpf.RGBBitCount)
        {
            if (SLUG_ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
            {
                return ImageFormat::FORMAT_R8G8B8A8_SNORM;
            }
            if (SLUG_ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
            {
                return ImageFormat::FORMAT_R16G16_SNORM;
            }
        }
    } else if (ddpf.flags & DDS_FOURCC)
    {
        if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
        {
            return forceSRGB ? ImageFormat::FORMAT_BC1_UNORM_SRGB : ImageFormat::FORMAT_BC1_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
        {
            return forceSRGB ? ImageFormat::FORMAT_BC2_UNORM_SRGB : ImageFormat::FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
        {
            return forceSRGB ? ImageFormat::FORMAT_BC3_UNORM_SRGB : ImageFormat::FORMAT_BC3_UNORM;
        }

        if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
        {
            return ImageFormat::FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
        {
            return ImageFormat::FORMAT_BC3_UNORM;
        }

        if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
        {
            return ImageFormat::FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
        {
            return ImageFormat::FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
        {
            return ImageFormat::FORMAT_BC4_SNORM;
        }

        if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
        {
            return ImageFormat::FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
        {
            return ImageFormat::FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
        {
            return ImageFormat::FORMAT_BC5_SNORM;
        }

        /*
        if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC)
        {
            return DXGI_FORMAT::DXGI_FORMAT_R8G8_B8G8_UNORM;
        }
        if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC)
        {
            return DXGI_FORMAT::DXGI_FORMAT_G8R8_G8B8_UNORM;
        }

        if (MAKEFOURCC('Y', 'U', 'Y', '2') == ddpf.fourCC)
        {
            return DXGI_FORMAT::DXGI_FORMAT_YUY2;
        }
        */

        switch (ddpf.fourCC)
        {
        case 36: // D3DFMT_A16B16G16R16
            return ImageFormat::FORMAT_R16G16B16A16_UNORM;

        case 110: // D3DFMT_Q16W16V16U16
            return ImageFormat::FORMAT_R16G16B16A16_SNORM;

        case 111: // D3DFMT_R16F
            return ImageFormat::FORMAT_R16_FLOAT;

        case 112: // D3DFMT_G16R16F
            return ImageFormat::FORMAT_R16G16_FLOAT;

        case 113: // D3DFMT_A16B16G16R16F
            return ImageFormat::FORMAT_R16G16B16A16_FLOAT;

        case 114: // D3DFMT_R32F
            return ImageFormat::FORMAT_R32_FLOAT;

        case 115: // D3DFMT_G32R32F
            return ImageFormat::FORMAT_R32G32B32_FLOAT;

        case 116: // D3DFMT_A32B32G32R32F
            return ImageFormat::FORMAT_R32G32B32A32_FLOAT;
        }
    }

    return ImageFormat::FORMAT_UNKNOWN;
}

ImageAlphaMode ConvertImageAlphaMode(const DDS_HEADER* header)
{
    if (header->ddspf.flags & DDS_FOURCC)
    {
        if (MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC)
        {
            auto d3d10ext = reinterpret_cast<const DDS_HEADER_DXT10*>(reinterpret_cast<const uint8_t*>(header) + sizeof(DDS_HEADER));
            auto mode = static_cast<ImageAlphaMode>(d3d10ext->miscFlags2 & DDS_MISC_FLAGS2_ALPHA_MODE_MASK);
            switch (mode)
            {
            case ImageAlphaMode::STRAIGHT:
            case ImageAlphaMode::PREMULTIPLIED:
            case ImageAlphaMode::OPAQUE_:
            case ImageAlphaMode::CUSTOM:
                return mode;

            default:
                break;
            }
        } else if ((MAKEFOURCC('D', 'X', 'T', '2') == header->ddspf.fourCC)
            || (MAKEFOURCC('D', 'X', 'T', '4') == header->ddspf.fourCC))
        {
            return ImageAlphaMode::PREMULTIPLIED;
        }
    }

    return ImageAlphaMode::UNKNOWN;
}

uint32_t GetBitsPerPixel(ImageFormat format)
{
    const ImageFormatMapping& mapping = IMAGE_FORMA_MAPPING[static_cast<uint32_t>(format)];
    return mapping.bitsPerPixel;
}

void ParseSurfaceInfo(size_t width,
    size_t height,
    ImageFormat fmt,
    uint32_t bitsPerPixel,
    size_t* outNumBytes,
    size_t* outRowBytes,
    size_t* outNumRows)
{
    size_t numBytes = 0;
    size_t rowBytes = 0;
    size_t numRows = 0;

    // bc圧縮かどうか、圧縮された4x4ブロックのバイト数を取得
    bool bc = false;
    size_t bpe = 0;
    {
        switch (fmt)
        {
        case ImageFormat::FORMAT_BC1_UNORM:
        case ImageFormat::FORMAT_BC1_UNORM_SRGB:
        case ImageFormat::FORMAT_BC4_UNORM:
        case ImageFormat::FORMAT_BC4_SNORM:
            bc = true;
            bpe = 8;
            break;

        case ImageFormat::FORMAT_BC2_UNORM:
        case ImageFormat::FORMAT_BC2_UNORM_SRGB:
        case ImageFormat::FORMAT_BC3_UNORM:
        case ImageFormat::FORMAT_BC3_UNORM_SRGB:
        case ImageFormat::FORMAT_BC5_UNORM:
        case ImageFormat::FORMAT_BC5_SNORM:
        case ImageFormat::FORMAT_BC6H_UF16:
        case ImageFormat::FORMAT_BC6H_SF16:
        case ImageFormat::FORMAT_BC7_UNORM:
        case ImageFormat::FORMAT_BC7_UNORM_SRGB:
            bc = true;
            bpe = 16;
            break;

        default:
            break;
        }
    }

    // bc圧縮の場合、圧縮されたピクセルブロックをもとにバイト数を計算する。
    if (bc)
    {
        // ピクセルブロックを計算
        // ピクセルブロックは4x4なので、4を除算
        // 3加算しているのは小数点切り上げ処理の代わり
        size_t numBlocksWide = 0;
        if (width > 0)
        {
            numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
        }
        size_t numBlocksHigh = 0;
        if (height > 0)
        {
            numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
        }
        rowBytes = numBlocksWide * bpe;
        numRows = numBlocksHigh;
        numBytes = rowBytes * numBlocksHigh;
    } else
    {
        // 圧縮じゃない場合は、そのまま計算
        // 7を加算しているのは小数点切り上げ処理の代わり
        rowBytes = (width * bitsPerPixel + 7) / 8;
        numRows = height;
        numBytes = rowBytes * height;
    }

    if (outNumBytes)
    {
        *outNumBytes = numBytes;
    }

    if (outRowBytes)
    {
        *outRowBytes = rowBytes;
    }

    if (outNumRows)
    {
        *outNumRows = numRows;
    }
}

size_t FillImageInfoOffset(ImageUtility::ImageInfo& imageInfo, size_t dataSize, ptrdiff_t dataOffset)
{
    imageInfo.originalBitsPerPixel = GetBitsPerPixel(imageInfo.format);

    // テクスチャ配列情報、ミップ情報を取得
    for (uint32_t arraySlice = 0; arraySlice < imageInfo.arraySize; arraySlice++)
    {
        size_t w = imageInfo.width;
        size_t h = imageInfo.height;
        size_t d = imageInfo.depth;

        if (arraySlice >= SLUG_IMAGE_MAX_ARRAY_SIZE)
        {
            break;
        }

        ImageUtility::ImageSubresourceInfo* sliceData = imageInfo.subresources[arraySlice];
        for (uint32_t mipLevel = 0; mipLevel < imageInfo.mipLevels; mipLevel++)
        {
            if (mipLevel >= SLUG_IMAGE_MAX_MIP_SIZE)
            {
                break;
            }

            size_t NumBytes = 0;
            size_t RowBytes = 0;
            size_t NumRows = 0;
            ParseSurfaceInfo(w, h, imageInfo.format, imageInfo.originalBitsPerPixel, &NumBytes, &RowBytes, &NumRows);

            ImageUtility::ImageSubresourceInfo& levelData = sliceData[mipLevel];
            levelData.dataOffset = dataOffset;
            levelData.dataSize = NumBytes;
            levelData.rowPitch = RowBytes;
            levelData.depthPitch = RowBytes * NumRows;

            dataOffset += NumBytes * d;

            // オフセット量がサイズを超えた場合終了
            if (dataSize > 0 && dataOffset > static_cast<ptrdiff_t>(dataSize))
            {
                return 0;
            }

            // mipmapなので、1になるまで2の累乗分減らす
            w = w >> 1;
            h = h >> 1;
            d = d >> 1;
            if (w == 0) w = 1;
            if (h == 0) h = 1;
            if (d == 0) d = 1;
        }
    }

    return dataOffset;
}

bool ParseImageInfo(ImageUtility::ImageInfo& imageInfo, void* data, size_t size, bool forceSrgb)
{
    if (!data && size < 1)
    {
        return false;
    }

    uint32_t dwMagicNumber = 0;
    memcpy(&dwMagicNumber, data, sizeof(uint32_t));

    if (dwMagicNumber != DDS_MAGIC)
    {
        return false;
    }

    const DDS_HEADER* header = reinterpret_cast<const DDS_HEADER*>(static_cast<const char*>(data) + sizeof(uint32_t));
    if (header == nullptr)
    {
        return false;
    }

    if (header->size != sizeof(DDS_HEADER) || header->ddspf.size != sizeof(DDS_PIXELFORMAT))
    {
        return false;
    }

    bool isDXT10Header = false;
    if (header->ddspf.flags & DDS_FOURCC && MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC)
    {
        if (size < (sizeof(DDS_HEADER) + sizeof(uint32_t) + sizeof(DDS_HEADER_DXT10)))
        {
            return false;
        }
        isDXT10Header = true;
    }

    ptrdiff_t dataOffset = sizeof(uint32_t) + sizeof(DDS_HEADER) + (isDXT10Header ? sizeof(DDS_HEADER_DXT10) : 0);

    imageInfo.width = header->width;
    imageInfo.height = header->height;
    imageInfo.mipLevels = header->mipMapCount ? header->mipMapCount : 1;
    imageInfo.depth = 1;
    imageInfo.arraySize = 1;
    imageInfo.alphaMode = ConvertImageAlphaMode(header);

    if (isDXT10Header)
    {
        const DDS_HEADER_DXT10* d3d10ext = reinterpret_cast<const DDS_HEADER_DXT10*>(reinterpret_cast<const char*>(header) + sizeof(DDS_HEADER));

        // 配列サイズが0(テクスチャが存在しない)場合失敗
        if (d3d10ext->arraySize == 0)
        {
            return false;
        }

        // フォーマット関連付け情報からフォーマットを取得
        for (const ImageFormatMapping& mapping : IMAGE_FORMA_MAPPING)
        {
            if (mapping.dxgiFormat == d3d10ext->dxgiFormat)
            {
                imageInfo.format = mapping.imageFormat;
                break;
            }
        }

        // フォーマットが不明な場合失敗
        if (imageInfo.format == ImageFormat::FORMAT_UNKNOWN)
        {
            return false;
        }

        // srgb強制モードなら適切なフォーマットに変更する
        if (forceSrgb)
        {
            switch (imageInfo.format)  // NOLINT(clang-diagnostic-switch-enum)
            {
            case(ImageFormat::FORMAT_R8G8B8A8_UNORM):
                imageInfo.format = ImageFormat::FORMAT_R8G8B8A8_UNORM_SRGB;
                break;

            case(ImageFormat::FORMAT_B8G8R8A8_UNORM):
                imageInfo.format = ImageFormat::FORMAT_B8G8R8A8_UNORM_SRGB;
                break;

            case(ImageFormat::FORMAT_BC1_UNORM):
                imageInfo.format = ImageFormat::FORMAT_BC1_UNORM_SRGB;
                break;

            case(ImageFormat::FORMAT_BC2_UNORM):
                imageInfo.format = ImageFormat::FORMAT_BC2_UNORM_SRGB;
                break;

            case(ImageFormat::FORMAT_BC3_UNORM):
                imageInfo.format = ImageFormat::FORMAT_BC3_UNORM_SRGB;
                break;

            case(ImageFormat::FORMAT_BC7_UNORM):
                imageInfo.format = ImageFormat::FORMAT_BC7_UNORM_SRGB;
                break;

            default:
                break;
            }
        }

        // テクスチャの次元数に合わせて、情報を設定
        if (d3d10ext->resourceDimension == DDS_DIMENSION_TEXTURE1D)
        {
            if ((header->flags & DDS_HEIGHT) && imageInfo.height != 1)
            {
                return false;
            }

            imageInfo.height = 1;
            imageInfo.dimension = d3d10ext->arraySize > 1 ? ImageDimension::Texture1DArray : ImageDimension::Texture1D;
        } else if (d3d10ext->resourceDimension == DDS_DIMENSION_TEXTURE2D)
        {
            if (d3d10ext->miscFlag & SLUG_D3D11_RESOURCE_MISC_TEXTURECUBE)
            {
                // キューブテクスチャの時,6面用意
                imageInfo.arraySize = d3d10ext->arraySize * 6;
                imageInfo.dimension = d3d10ext->arraySize > 1 ? ImageDimension::TextureCubeArray : ImageDimension::TextureCube;
            } else
            {
                imageInfo.arraySize = d3d10ext->arraySize;
                imageInfo.dimension = d3d10ext->arraySize > 1 ? ImageDimension::Texture2DArray : ImageDimension::Texture2D;
            }
        } else if (d3d10ext->resourceDimension == DDS_DIMENSION_TEXTURE3D)
        {
            // ボリュームテクスチャの場合
            if (!(header->flags & DDS_HEADER_FLAGS_VOLUME))
            {
                return false;
            }
            imageInfo.depth = header->depth;
            imageInfo.dimension = ImageDimension::Texture3D;
        } else
        {
            return false;
        }
    } else
    {
        imageInfo.format = ConvertImageFormat(header->ddspf, forceSrgb);

        // テクスチャフォーマットが不明の場合失敗
        if (imageInfo.format == ImageFormat::FORMAT_UNKNOWN)
        {
            return false;
        }

        if (header->flags & DDS_HEADER_FLAGS_VOLUME)
        {
            imageInfo.depth = header->depth;
            imageInfo.dimension = ImageDimension::Texture3D;
        } else
        {
            if (header->caps2 & DDS_CUBEMAP)
            {
                if ((header->caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
                {
                    return false;
                }

                imageInfo.arraySize = 6;
                imageInfo.dimension = ImageDimension::TextureCube;
            } else
            {
                imageInfo.dimension = ImageDimension::Texture2D;
            }
        }
    }
    if (FillImageInfoOffset(imageInfo, size, dataOffset) == 0)
    {
        return false;
    }

    imageInfo.data = data;
    imageInfo.size = size;

    return true;
}

bool ImageUtility::Parse(const InputParam& input, OutParam& param)
{
    return ParseImageInfo(param.imageInfo, input.data, input.size, input.forceSrgb);
}

}
}
