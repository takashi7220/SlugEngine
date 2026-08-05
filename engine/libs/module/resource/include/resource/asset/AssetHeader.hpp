#pragma once

#include <cstdint>
#include "resource/pack/PackHeader.hpp"
#include "core/container/Vector.hpp"
#include "core/container/String.hpp"

namespace slug::resource
{

using AssetTypeId = uint64_t;

struct AssetHeader
{
    AssetId assetId = s_InvalidAssetId;
    AssetTypeId typeId = 0;
    uint16_t version = 0;
};

class AssetHeaderUtility
{
public:
    static AssetTypeId MakeTypeId(core::StringView typeName);

    static bool Serialize(
        const AssetHeader& header,
        const core::TVector<ChunkId>& contentChunks,
        core::TVector<uint8_t>& out);

    static bool Deserialize(
        const void* data,
        uint32_t dataSize,
        AssetHeader& outHeader,
        core::TVector<ChunkId>& outContentChunks);
};
}
