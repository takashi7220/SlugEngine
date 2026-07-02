#include "resource/compress/IDecompressor.hpp"
#include "resource/compress/NullCodec.hpp"
#include "resource/compress/RLECodec.hpp"

namespace slug::resource
{

core::TUniquePtr<IDecompressor> IDecompressor::Create(CompressionType type)
{
    switch (type)
    {
    case CompressionType::None:
        return core::MakeUnique<NullDecompressor>();
    case CompressionType::RLE:
        return core::MakeUnique<RLEDecompressor>();
    }

    return nullptr;
}
}
