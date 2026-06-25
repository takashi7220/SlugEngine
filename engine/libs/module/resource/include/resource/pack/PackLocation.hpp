#pragma once

#include "resource/pack/PackHeader.hpp"
#include "resource/pack/MountedPack.hpp"
#include "core/filesystem/IFileHandle.hpp"

namespace slug::resource
{
struct ChunkLocation
{
    PackId packId = s_InvalidPackId;
    const ChunkRecord* record = nullptr;
    core::FileHandlePtr file;
    uint64_t fileOffset = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
    CompressionType compression = CompressionType::None;
};

struct AssetLocation
{
    PackId packId = s_InvalidPackId;
    const MountedPack* pack = nullptr;
    const AssetRecord* record = nullptr;
};

}
