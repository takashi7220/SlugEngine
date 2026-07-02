#include "resource/compress/RLECodec.hpp"
#include <cstring>

namespace slug::resource
{

namespace
{
constexpr uint32_t s_MaxRunLength = 255;
}

CompressionType RLECompressor::GetType() const
{
    return CompressionType::RLE;
}

bool RLECompressor::Compress(const void* src, uint32_t srcSize, core::TVector<uint8_t>& out) const
{
    out.clear();

    if (srcSize == 0)
    {
        return true;
    }

    if (src == nullptr)
    {
        return false;
    }

    const uint8_t* bytes = static_cast<const uint8_t*>(src);
    out.reserve(srcSize);

    uint32_t i = 0;
    while (i < srcSize)
    {
        const uint8_t value = bytes[i];
        uint32_t runLength = 1;
        while (runLength < s_MaxRunLength && i + runLength < srcSize && bytes[i + runLength] == value)
        {
            ++runLength;
        }

        out.push_back(static_cast<uint8_t>(runLength));
        out.push_back(value);
        i += runLength;
    }

    return true;
}

CompressionType RLEDecompressor::GetType() const
{
    return CompressionType::RLE;
}

bool RLEDecompressor::Decompress(const void* src, uint32_t srcSize, void* dst, uint32_t dstSize) const
{
    if (srcSize == 0)
    {
        return dstSize == 0;
    }

    if (src == nullptr || dst == nullptr)
    {
        return false;
    }

    if (srcSize % 2 != 0)
    {
        return false;
    }

    const uint8_t* bytes = static_cast<const uint8_t*>(src);
    uint8_t* out = static_cast<uint8_t*>(dst);

    uint32_t written = 0;
    for (uint32_t i = 0; i < srcSize; i += 2)
    {
        const uint8_t count = bytes[i];
        const uint8_t value = bytes[i + 1];

        if (written + count > dstSize)
        {
            return false;
        }

        std::memset(out + written, value, count);
        written += count;
    }

    return written == dstSize;
}
}
