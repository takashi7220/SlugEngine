#include "resource/asset/AssetHeader.hpp"
#include "core/utility/Hash.hpp"
#include <cstring>
#include <limits>

namespace slug::resource
{

AssetTypeId AssetHeaderUtility::MakeTypeId(core::StringView typeName)
{
    return core::HashUtility::ConvertStringToHash64(typeName);
}

bool AssetHeaderUtility::Serialize(
    const AssetHeader& header,
    const core::TVector<ChunkId>& contentChunks,
    core::TVector<uint8_t>& out)
{
    if (contentChunks.size() > std::numeric_limits<uint32_t>::max())
    {
        return false;
    }

    const uint32_t chunkCount = static_cast<uint32_t>(contentChunks.size());
    const size_t totalSize = sizeof(AssetHeader) + sizeof(uint32_t) + static_cast<size_t>(chunkCount) * sizeof(ChunkId);

    out.resize(totalSize);
    uint8_t* cursor = out.data();

    core::MemoryUtility::Memcpy(cursor, sizeof(AssetHeader), &header, sizeof(AssetHeader));
    cursor += sizeof(AssetHeader);

    core::MemoryUtility::Memcpy(cursor, sizeof(uint32_t), &chunkCount, sizeof(uint32_t));
    cursor += sizeof(uint32_t);

    if (chunkCount > 0)
    {
        core::MemoryUtility::Memcpy(cursor, static_cast<size_t>(chunkCount) * sizeof(ChunkId), contentChunks.data(), static_cast<size_t>(chunkCount) * sizeof(ChunkId));
    }

    return true;
}

bool AssetHeaderUtility::Deserialize(
    const void* data,
    uint32_t dataSize,
    AssetHeader& outHeader,
    core::TVector<ChunkId>& outContentChunks)
{
    if (data == nullptr)
    {
        return false;
    }

    if (dataSize < sizeof(AssetHeader) + sizeof(uint32_t))
    {
        return false;
    }

    const uint8_t* cursor = static_cast<const uint8_t*>(data);

    core::MemoryUtility::Memcpy(&outHeader, sizeof(AssetHeader), cursor, sizeof(AssetHeader));
    cursor += sizeof(AssetHeader);

    uint32_t chunkCount = 0;
    core::MemoryUtility::Memcpy(&chunkCount, sizeof(uint32_t), cursor, sizeof(uint32_t));
    cursor += sizeof(uint32_t);

    const size_t expectedSize = sizeof(AssetHeader) + sizeof(uint32_t) + static_cast<size_t>(chunkCount) * sizeof(ChunkId);
    if (dataSize != expectedSize)
    {
        return false;
    }

    outContentChunks.resize(chunkCount);
    if (chunkCount > 0)
    {
        core::MemoryUtility::Memcpy(outContentChunks.data(), static_cast<size_t>(chunkCount) * sizeof(ChunkId), cursor, static_cast<size_t>(chunkCount) * sizeof(ChunkId));
    }

    return true;
}
}
