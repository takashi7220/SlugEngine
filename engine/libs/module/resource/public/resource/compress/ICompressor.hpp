#pragma once

#include <cstdint>
#include "resource/pack/PackHeader.hpp"
#include "core/container/Vector.hpp"
#include "core/memory/UniquePtr.hpp"

namespace slug::resource
{

class ICompressor
{
public:
    virtual ~ICompressor() = default;

    ICompressor(const ICompressor&) = delete;
    ICompressor& operator=(const ICompressor&) = delete;

    virtual CompressionType GetType() const = 0;

    virtual bool Compress(const void* src, uint32_t srcSize, core::TVector<uint8_t>& out) const = 0;

    static core::TUniquePtr<ICompressor> Create(CompressionType type);

protected:
    ICompressor() = default;
};
}
