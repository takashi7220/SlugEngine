#include <gtest/gtest.h>

#include <resource/builder/Builder.hpp>
#include <resource/compress/IDecompressor.hpp>
#include <resource/pack/PackFileSystem.hpp>

#include <cstring>
#include <filesystem>
#include <string>

using namespace slug;

namespace
{

class TempPackDirectory
{
public:
    explicit TempPackDirectory(core::StringView testName)
    {
        m_path = std::filesystem::temp_directory_path() / "SlugEngineResourcePackUnit" / std::string(testName);

        std::error_code error;
        std::filesystem::remove_all(m_path, error);
        std::filesystem::create_directories(m_path);
    }

    ~TempPackDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    TempPackDirectory(const TempPackDirectory&) = delete;
    TempPackDirectory& operator=(const TempPackDirectory&) = delete;

    core::String MakePath(const char* filename) const
    {
        const std::string path = (m_path / filename).string();
        return core::String(path.c_str());
    }

private:
    std::filesystem::path m_path;
};

struct PackPaths
{
    core::String tocPath;
    core::String dataPath;
};

template<typename T>
T ReadStruct(const core::TVector<uint8_t>& buffer, size_t offset)
{
    T value = {};
    std::memcpy(&value, buffer.data() + offset, sizeof(T));
    return value;
}

void ExpectBytes(const core::TVector<uint8_t>& buffer, size_t offset, const core::TVector<uint8_t>& expected)
{
    ASSERT_LE(offset + expected.size(), buffer.size());
    EXPECT_EQ(0, std::memcmp(buffer.data() + offset, expected.data(), expected.size()));
}

resource::BuilderChunkDesc MakeChunkDesc(
    resource::ChunkId chunkId,
    const core::TVector<uint8_t>& payload,
    resource::CompressionType compression = resource::CompressionType::None,
    uint32_t alignment = 1)
{
    resource::BuilderChunkDesc desc = {};
    desc.chunkId = chunkId;
    desc.data = payload.data();
    desc.size = static_cast<uint32_t>(payload.size());
    desc.compression = compression;
    desc.alignment = alignment;
    return desc;
}

resource::BuilderAssetDesc MakeAssetDesc(
    resource::AssetId assetId,
    resource::ChunkId metadataChunkId,
    core::StringView name,
    const core::TVector<resource::AssetId>& dependencies = {})
{
    resource::BuilderAssetDesc desc = {};
    desc.assetId = assetId;
    desc.metadataChunkId = metadataChunkId;
    desc.version = 7;
    desc.name = core::String(name.data(), name.size());
    desc.dependencies = dependencies;
    return desc;
}

void BuildPackFiles(
    const PackPaths& paths,
    resource::AssetId assetId,
    resource::ChunkId chunkId,
    const core::TVector<uint8_t>& payload,
    core::StringView name,
    const core::TVector<resource::AssetId>& dependencies = {})
{
    resource::Builder builder;
    ASSERT_TRUE(builder.AddChunk(MakeChunkDesc(chunkId, payload)));
    ASSERT_TRUE(builder.AddAsset(MakeAssetDesc(assetId, chunkId, name, dependencies)));
    ASSERT_TRUE(builder.Build(paths.tocPath.c_str(), paths.dataPath.c_str()));
}

}

TEST(ResourceBuilder, RejectsInvalidChunkAndAssetDescriptors)
{
    const core::TVector<uint8_t> payload = { 1, 2, 3, 4 };

    resource::Builder builder;

    resource::BuilderChunkDesc invalidChunk = MakeChunkDesc(resource::s_InvalidChunkId, payload);
    EXPECT_FALSE(builder.AddChunk(invalidChunk));

    invalidChunk = MakeChunkDesc(101, payload);
    invalidChunk.data = nullptr;
    EXPECT_FALSE(builder.AddChunk(invalidChunk));

    invalidChunk = MakeChunkDesc(101, payload);
    invalidChunk.size = 0;
    EXPECT_FALSE(builder.AddChunk(invalidChunk));

    invalidChunk = MakeChunkDesc(101, payload);
    invalidChunk.alignment = 0;
    EXPECT_FALSE(builder.AddChunk(invalidChunk));

    const resource::BuilderChunkDesc chunk = MakeChunkDesc(101, payload, resource::CompressionType::None, 16);
    EXPECT_TRUE(builder.AddChunk(chunk));
    EXPECT_FALSE(builder.AddChunk(chunk));

    resource::BuilderAssetDesc invalidAsset = MakeAssetDesc(resource::s_InvalidAssetId, 101, "invalid.asset");
    EXPECT_FALSE(builder.AddAsset(invalidAsset));

    invalidAsset = MakeAssetDesc(201, resource::s_InvalidChunkId, "invalid.asset");
    EXPECT_FALSE(builder.AddAsset(invalidAsset));

    const resource::BuilderAssetDesc asset = MakeAssetDesc(201, 101, "player.asset");
    EXPECT_TRUE(builder.AddAsset(asset));
    EXPECT_FALSE(builder.AddAsset(asset));

    resource::Builder missingChunkBuilder;
    EXPECT_TRUE(missingChunkBuilder.AddAsset(MakeAssetDesc(202, 999, "missing-chunk.asset")));

    core::TVector<uint8_t> tocBuffer;
    core::TVector<uint8_t> dataBuffer;
    EXPECT_FALSE(missingChunkBuilder.Build(tocBuffer, dataBuffer));
}

TEST(ResourceBuilder, BuildsTocAndDataTables)
{
    const core::TVector<uint8_t> metadataPayload = { 10, 20, 30, 40 };
    const core::TVector<uint8_t> payload = { 1, 3, 5, 7, 9 };

    resource::Builder builder;
    ASSERT_TRUE(builder.AddChunk(MakeChunkDesc(301, metadataPayload, resource::CompressionType::None, 16)));
    ASSERT_TRUE(builder.AddChunk(MakeChunkDesc(302, payload, resource::CompressionType::None, 4)));
    ASSERT_TRUE(builder.AddAsset(MakeAssetDesc(401, 301, "hero.asset", { 402 })));
    ASSERT_TRUE(builder.AddAsset(MakeAssetDesc(402, 302, "weapon.asset")));

    core::TVector<uint8_t> tocBuffer;
    core::TVector<uint8_t> dataBuffer;
    ASSERT_TRUE(builder.Build(tocBuffer, dataBuffer));

    ASSERT_GE(tocBuffer.size(), sizeof(resource::PackTocHeader));
    ASSERT_GE(dataBuffer.size(), sizeof(resource::PackDataHeader));

    const resource::PackTocHeader tocHeader = ReadStruct<resource::PackTocHeader>(tocBuffer, 0);
    EXPECT_EQ(0, std::memcmp(tocHeader.magic, resource::s_ExpectedPackMagic, sizeof(tocHeader.magic)));
    EXPECT_EQ(resource::s_PackFormatVersion, tocHeader.version);
    EXPECT_EQ(2u, tocHeader.assetCount);
    EXPECT_EQ(2u, tocHeader.chunkCount);
    EXPECT_EQ(1u, tocHeader.dependencyCount);
    EXPECT_EQ(sizeof(resource::AssetRecord), tocHeader.assetRecordSize);
    EXPECT_EQ(sizeof(resource::ChunkRecord), tocHeader.chunkRecordSize);
    EXPECT_EQ(sizeof(resource::DependencyRecord), tocHeader.dependencyRecordSize);
    EXPECT_EQ(dataBuffer.size(), tocHeader.dataFileSize);
    EXPECT_EQ(16u, tocHeader.alignment);

    const resource::PackDataHeader dataHeader = ReadStruct<resource::PackDataHeader>(dataBuffer, 0);
    EXPECT_EQ(0, std::memcmp(dataHeader.magic, resource::s_ExpectedPackDataMagic, sizeof(dataHeader.magic)));
    EXPECT_EQ(resource::s_PackFormatVersion, dataHeader.version);
    EXPECT_EQ(dataBuffer.size(), dataHeader.dataSize);

    const resource::AssetRecord heroAsset = ReadStruct<resource::AssetRecord>(tocBuffer, tocHeader.assetTableOffset);
    const resource::AssetRecord weaponAsset = ReadStruct<resource::AssetRecord>(
        tocBuffer,
        tocHeader.assetTableOffset + sizeof(resource::AssetRecord));

    EXPECT_EQ(401u, heroAsset.assetId);
    EXPECT_EQ(301u, heroAsset.metadataChunkId);
    EXPECT_EQ(0u, heroAsset.firstDependency);
    EXPECT_EQ(1u, heroAsset.dependencyCount);
    EXPECT_EQ(402u, weaponAsset.assetId);
    EXPECT_EQ(302u, weaponAsset.metadataChunkId);
    EXPECT_EQ(0u, weaponAsset.dependencyCount);

    const resource::DependencyRecord dependency = ReadStruct<resource::DependencyRecord>(
        tocBuffer,
        tocHeader.dependencyTableOffset);
    EXPECT_EQ(402u, dependency.dependencyAssetId);

    const char* stringTable = reinterpret_cast<const char*>(tocBuffer.data() + tocHeader.stringTableOffset);
    ASSERT_LT(heroAsset.nameOffset, tocHeader.stringTableSize);
    ASSERT_LT(weaponAsset.nameOffset, tocHeader.stringTableSize);
    EXPECT_STREQ("hero.asset", stringTable + heroAsset.nameOffset);
    EXPECT_STREQ("weapon.asset", stringTable + weaponAsset.nameOffset);

    const resource::ChunkRecord metadataChunk = ReadStruct<resource::ChunkRecord>(
        tocBuffer,
        tocHeader.chunkTableOffset);
    const resource::ChunkRecord dataChunk = ReadStruct<resource::ChunkRecord>(
        tocBuffer,
        tocHeader.chunkTableOffset + sizeof(resource::ChunkRecord));

    EXPECT_EQ(301u, metadataChunk.chunkId);
    EXPECT_EQ(0u, metadataChunk.offset % 16u);
    EXPECT_EQ(metadataPayload.size(), metadataChunk.compressedSize);
    EXPECT_EQ(metadataPayload.size(), metadataChunk.uncompressedSize);
    EXPECT_EQ(resource::CompressionType::None, metadataChunk.compression);
    ExpectBytes(dataBuffer, metadataChunk.offset, metadataPayload);

    EXPECT_EQ(302u, dataChunk.chunkId);
    EXPECT_EQ(0u, dataChunk.offset % 4u);
    EXPECT_EQ(payload.size(), dataChunk.compressedSize);
    EXPECT_EQ(payload.size(), dataChunk.uncompressedSize);
    EXPECT_EQ(resource::CompressionType::None, dataChunk.compression);
    ExpectBytes(dataBuffer, dataChunk.offset, payload);
}

TEST(ResourceBuilder, StoresCompressedChunksWithUncompressedSize)
{
    const core::TVector<uint8_t> payload = { 5, 5, 5, 5, 5, 9, 9, 1, 1, 1 };

    resource::Builder builder;
    ASSERT_TRUE(builder.AddChunk(MakeChunkDesc(501, payload, resource::CompressionType::RLE)));
    ASSERT_TRUE(builder.AddAsset(MakeAssetDesc(601, 501, "compressed.asset")));

    core::TVector<uint8_t> tocBuffer;
    core::TVector<uint8_t> dataBuffer;
    ASSERT_TRUE(builder.Build(tocBuffer, dataBuffer));

    const resource::PackTocHeader tocHeader = ReadStruct<resource::PackTocHeader>(tocBuffer, 0);
    const resource::ChunkRecord chunk = ReadStruct<resource::ChunkRecord>(tocBuffer, tocHeader.chunkTableOffset);

    EXPECT_EQ(resource::CompressionType::RLE, chunk.compression);
    EXPECT_EQ(payload.size(), chunk.uncompressedSize);
    EXPECT_LT(chunk.compressedSize, chunk.uncompressedSize);
    ASSERT_LE(chunk.offset + chunk.compressedSize, dataBuffer.size());

    core::TVector<uint8_t> restored(payload.size(), 0);
    auto decompressor = resource::IDecompressor::Create(chunk.compression);
    ASSERT_TRUE(decompressor);
    EXPECT_TRUE(decompressor->Decompress(
        dataBuffer.data() + chunk.offset,
        chunk.compressedSize,
        restored.data(),
        static_cast<uint32_t>(restored.size())));
    EXPECT_EQ(payload, restored);
}

TEST(ResourcePackFileSystem, MountsPackFindsAssetAndReadsChunk)
{
    TempPackDirectory tempDirectory("MountsPackFindsAssetAndReadsChunk");
    const PackPaths paths = {
        tempDirectory.MakePath("main.toc"),
        tempDirectory.MakePath("main.data"),
    };

    const resource::AssetId assetId = 701;
    const resource::AssetId dependencyAssetId = 702;
    const resource::ChunkId chunkId = 801;
    const core::TVector<uint8_t> payload = { 9, 8, 7, 6, 5 };

    BuildPackFiles(paths, assetId, chunkId, payload, "player.asset", { dependencyAssetId });

    resource::PackFileSystem packFileSystem;
    resource::MountDesc mountDesc = {};
    mountDesc.tocPath = paths.tocPath.c_str();
    mountDesc.dataPath = paths.dataPath.c_str();
    mountDesc.priority = 42;

    ASSERT_TRUE(packFileSystem.Mount(mountDesc));
    EXPECT_EQ(1u, packFileSystem.GetMountedPackCount());

    resource::AssetLocation assetLocation = {};
    ASSERT_TRUE(packFileSystem.TryFindAsset(assetId, assetLocation));
    ASSERT_NE(nullptr, assetLocation.pack);
    ASSERT_NE(nullptr, assetLocation.record);
    EXPECT_EQ(chunkId, assetLocation.record->metadataChunkId);
    EXPECT_EQ(7u, assetLocation.record->version);
    EXPECT_EQ(0u, assetLocation.record->firstDependency);
    EXPECT_EQ(1u, assetLocation.record->dependencyCount);
    ASSERT_LT(assetLocation.record->nameOffset, assetLocation.pack->stringTable.size());
    EXPECT_STREQ("player.asset", assetLocation.pack->stringTable.data() + assetLocation.record->nameOffset);

    ASSERT_EQ(1u, assetLocation.pack->dependencies.size());
    EXPECT_EQ(dependencyAssetId, assetLocation.pack->dependencies[0].dependencyAssetId);

    resource::ChunkLocation chunkLocation = {};
    ASSERT_TRUE(packFileSystem.TryFindChunk(chunkId, chunkLocation));
    ASSERT_TRUE(packFileSystem.TryFindChunkInPack(assetLocation.packId, chunkId, chunkLocation));
    EXPECT_EQ(42, chunkLocation.priority);
    EXPECT_EQ(payload.size(), chunkLocation.compressedSize);
    EXPECT_EQ(payload.size(), chunkLocation.uncompressedSize);
    EXPECT_EQ(resource::CompressionType::None, chunkLocation.compression);

    core::TVector<uint8_t> readBuffer(payload.size(), 0);
    ASSERT_TRUE(packFileSystem.ReadChunkSync(chunkLocation, readBuffer.data(), static_cast<uint32_t>(readBuffer.size())));
    EXPECT_EQ(payload, readBuffer);

    core::TVector<uint8_t> readByIdBuffer(payload.size(), 0);
    ASSERT_TRUE(packFileSystem.ReadChunkSync(chunkId, readByIdBuffer.data(), readByIdBuffer.size()));
    EXPECT_EQ(payload, readByIdBuffer);

    core::TVector<uint8_t> smallBuffer(payload.size() - 1, 0);
    EXPECT_FALSE(packFileSystem.ReadChunkSync(chunkLocation, smallBuffer.data(), static_cast<uint32_t>(smallBuffer.size())));
    EXPECT_FALSE(packFileSystem.TryFindAsset(9999, assetLocation));
    EXPECT_FALSE(packFileSystem.TryFindChunk(9999, chunkLocation));

    packFileSystem.UnmountAll();
    EXPECT_EQ(0u, packFileSystem.GetMountedPackCount());
}

TEST(ResourcePackFileSystem, ResolvesHighestPriorityPackFirst)
{
    TempPackDirectory tempDirectory("ResolvesHighestPriorityPackFirst");
    const PackPaths lowPriorityPaths = {
        tempDirectory.MakePath("low.toc"),
        tempDirectory.MakePath("low.data"),
    };
    const PackPaths highPriorityPaths = {
        tempDirectory.MakePath("high.toc"),
        tempDirectory.MakePath("high.data"),
    };

    const resource::AssetId assetId = 901;
    const resource::ChunkId chunkId = 1001;
    const core::TVector<uint8_t> lowPriorityPayload = { 1, 1, 1, 1 };
    const core::TVector<uint8_t> highPriorityPayload = { 2, 2, 2, 2 };

    BuildPackFiles(lowPriorityPaths, assetId, chunkId, lowPriorityPayload, "shared.asset");
    BuildPackFiles(highPriorityPaths, assetId, chunkId, highPriorityPayload, "shared.asset");

    resource::PackFileSystem packFileSystem;

    resource::MountDesc lowPriorityDesc = {};
    lowPriorityDesc.tocPath = lowPriorityPaths.tocPath.c_str();
    lowPriorityDesc.dataPath = lowPriorityPaths.dataPath.c_str();
    lowPriorityDesc.priority = 1;
    ASSERT_TRUE(packFileSystem.Mount(lowPriorityDesc));

    resource::MountDesc highPriorityDesc = {};
    highPriorityDesc.tocPath = highPriorityPaths.tocPath.c_str();
    highPriorityDesc.dataPath = highPriorityPaths.dataPath.c_str();
    highPriorityDesc.priority = 10;
    ASSERT_TRUE(packFileSystem.Mount(highPriorityDesc));

    ASSERT_EQ(2u, packFileSystem.GetMountedPackCount());

    resource::AssetLocation assetLocation = {};
    ASSERT_TRUE(packFileSystem.TryFindAsset(assetId, assetLocation));
    ASSERT_NE(nullptr, assetLocation.pack);
    EXPECT_EQ(10, assetLocation.pack->priority);

    core::TVector<uint8_t> readBuffer(highPriorityPayload.size(), 0);
    ASSERT_TRUE(packFileSystem.ReadChunkSync(chunkId, readBuffer.data(), readBuffer.size()));
    EXPECT_EQ(highPriorityPayload, readBuffer);

    packFileSystem.Umount(assetLocation.packId);
    ASSERT_EQ(1u, packFileSystem.GetMountedPackCount());

    readBuffer.assign(lowPriorityPayload.size(), 0);
    ASSERT_TRUE(packFileSystem.ReadChunkSync(chunkId, readBuffer.data(), readBuffer.size()));
    EXPECT_EQ(lowPriorityPayload, readBuffer);
}
