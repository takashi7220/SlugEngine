#pragma once

#include <cstdint>
#include "resource/pack/PackHeader.hpp"
#include "core/memory/UniquePtr.hpp"

namespace slug::resource
{

class IDecompressor
{
public:
    virtual ~IDecompressor() = default;

    IDecompressor(const IDecompressor&) = delete;
    IDecompressor& operator=(const IDecompressor&) = delete;

    virtual CompressionType GetType() const = 0;

    virtual bool Decompress(const void* src, uint32_t srcSize, void* dst, uint32_t dstSize) const = 0;

    static core::TUniquePtr<IDecompressor> Create(CompressionType type);

protected:
    IDecompressor() = default;
};
}
