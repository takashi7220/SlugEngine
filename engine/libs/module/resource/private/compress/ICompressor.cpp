#include "resource/compress/ICompressor.hpp"
#include "resource/compress/NullCodec.hpp"
#include "resource/compress/RLECodec.hpp"

namespace slug::resource
{

core::TUniquePtr<ICompressor> ICompressor::Create(CompressionType type)
{
    switch (type)
    {
    case CompressionType::None:
        return core::MakeUnique<NullCompressor>();
    case CompressionType::RLE:
        return core::MakeUnique<RLECompressor>();
    }

    return nullptr;
}
}
