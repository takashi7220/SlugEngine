#include "resource/compress/NullCodec.hpp"
#include <cstring>

namespace slug::resource
{

CompressionType NullCompressor::GetType() const
{
    return CompressionType::None;
}

bool NullCompressor::Compress(const void* src, uint32_t srcSize, core::TVector<uint8_t>& out) const
{
    if (srcSize > 0 && src == nullptr)
    {
        return false;
    }

    const uint8_t* bytes = static_cast<const uint8_t*>(src);
    out.assign(bytes, bytes + srcSize);
    return true;
}

CompressionType NullDecompressor::GetType() const
{
    return CompressionType::None;
}

bool NullDecompressor::Decompress(const void* src, uint32_t srcSize, void* dst, uint32_t dstSize) const
{
    if (srcSize != dstSize)
    {
        return false;
    }

    if (srcSize == 0)
    {
        return true;
    }

    if (src == nullptr || dst == nullptr)
    {
        return false;
    }

    std::memcpy(dst, src, srcSize);
    return true;
}
}
