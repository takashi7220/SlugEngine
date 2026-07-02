#pragma once

#include <cstdint>
#include "resource/pack/PackHeader.hpp"
#include "core/container/Vector.hpp"
#include "core/container/String.hpp"

namespace slug::resource
{

struct BuilderChunkDesc
{
    ChunkId chunkId = s_InvalidChunkId;
    const void* data = nullptr;
    uint32_t size = 0;
    CompressionType compression = CompressionType::None;
    uint32_t alignment = 1;
};

struct BuilderAssetDesc
{
    AssetId assetId = s_InvalidAssetId;
    ChunkId metadataChunkId = s_InvalidChunkId;
    uint16_t flags = 0;
    uint16_t version = 0;
    core::String name;
    core::TVector<AssetId> dependencies;
};

// PackFileSystem がマウントできる TOC ファイルとデータファイルの組を書き出すオフラインビルダー。
class Builder
{
public:
    bool AddChunk(const BuilderChunkDesc& desc);
    bool AddAsset(const BuilderAssetDesc& desc);

    bool Build(core::StringView tocPath, core::StringView dataPath) const;

private:
    struct PendingChunk
    {
        ChunkId chunkId = s_InvalidChunkId;
        core::TVector<uint8_t> compressedData;
        uint32_t uncompressedSize = 0;
        CompressionType compression = CompressionType::None;
        uint32_t alignment = 1;
    };

    struct PendingAsset
    {
        AssetId assetId = s_InvalidAssetId;
        ChunkId metadataChunkId = s_InvalidChunkId;
        uint16_t flags = 0;
        uint16_t version = 0;
        core::String name;
        core::TVector<AssetId> dependencies;
    };

    bool BuildDataFile(core::TVector<uint8_t>& outData, core::TVector<ChunkRecord>& outChunkRecords, uint32_t& outMaxAlignment) const;

    void BuildTables(
        core::TVector<AssetRecord>& outAssetRecords,
        core::TVector<DependencyRecord>& outDependencyRecords,
        core::TVector<char>& outStringTable) const;

    core::TVector<PendingChunk> m_chunks;
    core::TVector<PendingAsset> m_assets;
};
}
