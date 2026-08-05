#pragma once

#include "resource/compress/ICompressor.hpp"
#include "resource/compress/IDecompressor.hpp"

namespace slug::resource
{

class NullCompressor : public ICompressor
{
public:
    CompressionType GetType() const override;
    bool Compress(const void* src, uint32_t srcSize, core::TVector<uint8_t>& out) const override;
};

class NullDecompressor : public IDecompressor
{
public:
    CompressionType GetType() const override;
    bool Decompress(const void* src, uint32_t srcSize, void* dst, uint32_t dstSize) const override;
};
}
