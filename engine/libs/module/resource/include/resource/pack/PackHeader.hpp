#pragma once
#include <cstdint>

namespace slug::resource
{

using PackId = uint64_t;
using AssetId = uint64_t;
using ChunkId = uint64_t;

static constexpr uint32_t s_PackFormatVersion = 1;
static constexpr PackId s_InvalidPackId = 0;

enum class PackFlags : uint32_t
{
    PackFlag_None = 0,
    PackFlag_TocCompressed = 1 << 0,
    PackFlag_TocEncrypted = 1 << 1,
    PackFlag_DataEncrypted = 1 << 2,
    PackFlag_Signed = 1 << 3,
};

enum class CompressionType : uint8_t
{
    None,
    LZ4,
};

struct PackTocHeader
{
    char magic[4];
    uint32_t version;
    uint32_t flags;

    uint64_t assetTableOffset;
    uint32_t assetCount;
    uint32_t assetRecordSize;

    uint64_t chunkTableOffset;
    uint32_t chunkCount;
    uint32_t chunkRecordSize;

    uint64_t dependencyTableOffset;
    uint32_t dependencyCount;
    uint32_t dependencyRecordSize;

    uint64_t stringTableOffset;
    uint32_t stringTableSize;
    uint32_t reserved0;

    uint64_t dataFileSize;
    uint8_t buildId[16];

    uint32_t alignment;
    uint32_t compressionBlockSize;

    uint8_t tocHash[16];
    uint8_t reserved[64];
};

struct PackDataHeader
{
    char magic[8];
    uint32_t version;
    uint32_t flags;
    uint64_t dataSize;
    uint8_t buildId[16];
    uint8_t reserved[32];
};

struct AssetRecord
{
    AssetId assetId;
    ChunkId metadataChunkId;
    uint16_t flags;
    uint16_t version;
    uint32_t firstDependency;
    uint32_t dependencyCount;
    uint32_t nameOffset;
};

struct ChunkRecord
{
    ChunkId chunkId;
    uint64_t offset;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    CompressionType compression;
    uint16_t flags;
    uint16_t alignment;
    uint8_t contentHash[16];
};

struct DependencyRecord
{
    AssetId dependencyAssetId;
    uint16_t flags;
    uint16_t reserved;
};

}
