#include "resource/builder/Builder.hpp"
#include "resource/compress/ICompressor.hpp"
#include "core/filesystem/FileSystem.hpp"
#include "core/utility/Hash.hpp"
#include <algorithm>
#include <cstring>

namespace slug::resource
{

namespace
{

void ComputeHash128(core::StringView view, uint8_t out[16])
{
    const uint64_t first = core::HashUtility::ConvertStringToHash64(view);

    size_t seed = static_cast<size_t>(first);
    core::HashUtility::HashCombine(seed, view.size());
    const uint64_t second = static_cast<uint64_t>(seed);

    std::memcpy(out, &first, sizeof(first));
    std::memcpy(out + sizeof(first), &second, sizeof(second));
}

core::StringView AsView(const core::TVector<uint8_t>& bytes)
{
    return core::StringView(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}
}

bool Builder::AddChunk(const BuilderChunkDesc& desc)
{
    if (desc.chunkId == s_InvalidChunkId)
    {
        return false;
    }

    if (desc.size == 0 || desc.data == nullptr)
    {
        return false;
    }

    if (desc.alignment == 0 || desc.alignment > 0xFFFFu)
    {
        return false;
    }

    for (const auto& chunk : m_chunks)
    {
        if (chunk.chunkId == desc.chunkId)
        {
            return false;
        }
    }

    auto compressor = ICompressor::Create(desc.compression);
    if (!compressor)
    {
        return false;
    }

    PendingChunk chunk;
    chunk.chunkId = desc.chunkId;
    chunk.uncompressedSize = desc.size;
    chunk.compression = desc.compression;
    chunk.alignment = desc.alignment;

    if (!compressor->Compress(desc.data, desc.size, chunk.compressedData) || chunk.compressedData.empty())
    {
        return false;
    }

    m_chunks.push_back(std::move(chunk));
    return true;
}

bool Builder::AddAsset(const BuilderAssetDesc& desc)
{
    if (desc.assetId == s_InvalidAssetId || desc.metadataChunkId == s_InvalidChunkId)
    {
        return false;
    }

    for (const auto& asset : m_assets)
    {
        if (asset.assetId == desc.assetId)
        {
            return false;
        }
    }

    PendingAsset asset;
    asset.assetId = desc.assetId;
    asset.metadataChunkId = desc.metadataChunkId;
    asset.flags = desc.flags;
    asset.version = desc.version;
    asset.name = desc.name;
    asset.dependencies = desc.dependencies;

    m_assets.push_back(std::move(asset));
    return true;
}

bool Builder::BuildDataFile(core::TVector<uint8_t>& outData, core::TVector<ChunkRecord>& outChunkRecords, uint32_t& outMaxAlignment) const
{
    outData.clear();
    outChunkRecords.clear();
    outMaxAlignment = 1;

    const size_t headerSize = sizeof(PackDataHeader);
    outData.resize(headerSize, 0);

    for (const auto& chunk : m_chunks)
    {
        outMaxAlignment = std::max(outMaxAlignment, chunk.alignment);

        const size_t remainder = outData.size() % chunk.alignment;
        if (remainder != 0)
        {
            outData.resize(outData.size() + (chunk.alignment - remainder), 0);
        }

        ChunkRecord record = {};
        record.chunkId = chunk.chunkId;
        record.offset = outData.size();
        record.compressedSize = static_cast<uint32_t>(chunk.compressedData.size());
        record.uncompressedSize = chunk.uncompressedSize;
        record.compression = chunk.compression;
        record.flags = 0;
        record.alignment = static_cast<uint16_t>(chunk.alignment);
        ComputeHash128(AsView(chunk.compressedData), record.contentHash);

        outData.insert(outData.end(), chunk.compressedData.begin(), chunk.compressedData.end());
        outChunkRecords.push_back(record);
    }

    PackDataHeader dataHeader = {};
    std::memcpy(dataHeader.magic, s_ExpectedPackDataMagic, sizeof(dataHeader.magic));
    dataHeader.version = s_PackFormatVersion;
    dataHeader.flags = static_cast<uint32_t>(PackFlags::PackFlag_None);
    dataHeader.dataSize = outData.size();
    std::memset(dataHeader.reserved, 0, sizeof(dataHeader.reserved));
    ComputeHash128(
        core::StringView(reinterpret_cast<const char*>(outData.data() + headerSize), outData.size() - headerSize),
        dataHeader.buildId);

    std::memcpy(outData.data(), &dataHeader, headerSize);

    return true;
}

void Builder::BuildTables(
    core::TVector<AssetRecord>& outAssetRecords,
    core::TVector<DependencyRecord>& outDependencyRecords,
    core::TVector<char>& outStringTable) const
{
    outAssetRecords.clear();
    outDependencyRecords.clear();
    outStringTable.clear();

    for (const auto& asset : m_assets)
    {
        AssetRecord record = {};
        record.assetId = asset.assetId;
        record.metadataChunkId = asset.metadataChunkId;
        record.flags = asset.flags;
        record.version = asset.version;
        record.firstDependency = static_cast<uint32_t>(outDependencyRecords.size());
        record.dependencyCount = static_cast<uint32_t>(asset.dependencies.size());
        record.nameOffset = static_cast<uint32_t>(outStringTable.size());

        for (AssetId dependency : asset.dependencies)
        {
            DependencyRecord dependencyRecord = {};
            dependencyRecord.dependencyAssetId = dependency;
            dependencyRecord.flags = 0;
            dependencyRecord.reserved = 0;
            outDependencyRecords.push_back(dependencyRecord);
        }

        outStringTable.insert(outStringTable.end(), asset.name.begin(), asset.name.end());
        outStringTable.push_back('\0');

        outAssetRecords.push_back(record);
    }
}

bool Builder::Build(core::TVector<uint8_t>& outToc, core::TVector<uint8_t>& outData) const
{
    for (const auto& asset : m_assets)
    {
        const bool hasMetadataChunk = std::any_of(
            m_chunks.begin(),
            m_chunks.end(),
            [&](const PendingChunk& chunk)
            {
                return chunk.chunkId == asset.metadataChunkId;
            });

        if (!hasMetadataChunk)
        {
            return false;
        }
    }

    core::TVector<ChunkRecord> chunkRecords;
    uint32_t maxAlignment = 1;
    if (!BuildDataFile(outData, chunkRecords, maxAlignment))
    {
        return false;
    }

    core::TVector<AssetRecord> assetRecords;
    core::TVector<DependencyRecord> dependencyRecords;
    core::TVector<char> stringTable;
    BuildTables(assetRecords, dependencyRecords, stringTable);

    PackTocHeader header = {};
    std::memcpy(header.magic, s_ExpectedPackMagic, sizeof(header.magic));
    header.version = s_PackFormatVersion;
    header.flags = static_cast<uint32_t>(PackFlags::PackFlag_None);

    const uint64_t headerSize = sizeof(PackTocHeader);
    header.assetTableOffset = headerSize;
    header.assetCount = static_cast<uint32_t>(assetRecords.size());
    header.assetRecordSize = sizeof(AssetRecord);

    header.chunkTableOffset = header.assetTableOffset + static_cast<uint64_t>(assetRecords.size()) * sizeof(AssetRecord);
    header.chunkCount = static_cast<uint32_t>(chunkRecords.size());
    header.chunkRecordSize = sizeof(ChunkRecord);

    header.dependencyTableOffset = header.chunkTableOffset + static_cast<uint64_t>(chunkRecords.size()) * sizeof(ChunkRecord);
    header.dependencyCount = static_cast<uint32_t>(dependencyRecords.size());
    header.dependencyRecordSize = sizeof(DependencyRecord);

    header.stringTableOffset = header.dependencyTableOffset + static_cast<uint64_t>(dependencyRecords.size()) * sizeof(DependencyRecord);
    header.stringTableSize = static_cast<uint32_t>(stringTable.size());
    header.reserved0 = 0;

    header.dataFileSize = outData.size();
    header.alignment = maxAlignment;
    header.compressionBlockSize = 0;
    std::memset(header.buildId, 0, sizeof(header.buildId));
    std::memset(header.reserved, 0, sizeof(header.reserved));

    outToc.clear();
    outToc.resize(static_cast<size_t>(header.stringTableOffset) + header.stringTableSize, 0);

    if (!assetRecords.empty())
    {
        std::memcpy(outToc.data() + header.assetTableOffset, assetRecords.data(), assetRecords.size() * sizeof(AssetRecord));
    }
    if (!chunkRecords.empty())
    {
        std::memcpy(outToc.data() + header.chunkTableOffset, chunkRecords.data(), chunkRecords.size() * sizeof(ChunkRecord));
    }
    if (!dependencyRecords.empty())
    {
        std::memcpy(outToc.data() + header.dependencyTableOffset, dependencyRecords.data(), dependencyRecords.size() * sizeof(DependencyRecord));
    }
    if (!stringTable.empty())
    {
        std::memcpy(outToc.data() + header.stringTableOffset, stringTable.data(), stringTable.size());
    }

    // tocHash はヘッダ自身を含めると自己参照になるため、ヘッダ以降のテーブル部分のみを対象に計算する。
    ComputeHash128(
        core::StringView(reinterpret_cast<const char*>(outToc.data() + headerSize), outToc.size() - headerSize),
        header.tocHash);

    std::memcpy(outToc.data(), &header, headerSize);

    return true;
}

bool Builder::Build(core::StringView tocPath, core::StringView dataPath) const
{
    core::TVector<uint8_t> tocBuffer;
    core::TVector<uint8_t> dataBlob;
    if (!Build(tocBuffer, dataBlob))
    {
        return false;
    }

    if (!core::FileSystem::Write(tocPath, tocBuffer.size(), tocBuffer.data()))
    {
        return false;
    }

    return core::FileSystem::Write(dataPath, dataBlob.size(), dataBlob.data());
}
}
