#include "resource/pack/PackFileSystem.hpp"
#include "core/filesystem/FileSystem.hpp"
#include <algorithm>
#include <cstring>
#include <limits>

namespace slug::resource
{
namespace
{

bool IsValidMagic(const char magic[4])
{
    return std::memcmp(magic, s_ExpectedPackMagic, sizeof(s_ExpectedPackMagic)) == 0;
}

template<class T>
bool ReadTable(const core::FileHandlePtr& file, uint64_t offset, uint32_t count, uint32_t recordSize, core::TVector<T>& outTable)
{
    if (count == 0)
    {
        outTable.clear();
        return true;
    }

    if (recordSize != sizeof(T))
    {
        return false;
    }

    uint64_t bytes64 = count * recordSize;
    if (bytes64 > std::numeric_limits<uint32_t>::max())
    {
        return false;
    }

    outTable.resize(count);

    return file->ReadAt(offset, outTable.data(), static_cast<uint32_t>(bytes64));
}
}


PackFileSystem::PackFileSystem()
{
    core::LockGuard<core::Mutex> lock(m_snapshotMutex);
    m_snapshot = core::MakeReference<Snapshot>();
}

PackFileSystem::~PackFileSystem()
{
    UnmountAll();
}

bool PackFileSystem::Mount(const MountDesc& desc)
{
    MountedPackPtr pack = core::MakeReference<MountedPack>();

    pack->id = m_nextPackId.fetch_add(1);
    pack->tocPath = desc.tocPath;
    pack->dataPath = desc.dataPath;
    pack->priority = desc.priority;

    if (!LoadToc(desc.tocPath.data(), *pack))
    {
        return false;
    }

    pack->dataFile = core::FileSystem::Open({ .path = desc.dataPath.data(), .async = true, .randomAccess = true });
    if (!pack->dataFile || !pack->dataFile->IsValid())
    {
        return false;
    }

    if (!BuildIndex(*pack))
    {
        return false;
    }

    core::LockGuard<core::Mutex> lock(m_snapshotMutex);

    auto newSnapshot = core::MakeReference<Snapshot>();
    if (m_snapshot)
    {
        newSnapshot->packs = m_snapshot->packs;
    }

    newSnapshot->packs.emplace_back(std::move(pack));
    SortByPriority(newSnapshot->packs);

    m_snapshot = newSnapshot;
    return true;
}

void PackFileSystem::Umount(PackId packId)
{
    core::LockGuard<core::Mutex> lock(m_snapshotMutex);
    if (!m_snapshot)
    {
        return;
    }

    auto newSnapshot = core::MakeReference<Snapshot>();
    newSnapshot->packs.reserve(m_snapshot->packs.size());

    for (const auto& pack : m_snapshot->packs)
    {
        if (pack->id == packId)
        {
            if (pack->activeReadCount == 0 && pack->referencedResourceCount == 0)
            {
                pack->dataFile.reset();
                pack->state = MountedPackState::Unmounted;
                continue;
            }

            pack->state = MountedPackState::PendingUnmount;
        }

        newSnapshot->packs.push_back(pack);
    }

    m_snapshot = newSnapshot;
}

void PackFileSystem::UnmountAll()
{
    core::LockGuard<core::Mutex> lock(m_snapshotMutex);
    m_snapshot = core::MakeReference<Snapshot>();
}

PackFileSystem::SnapshotPtr PackFileSystem::GetSnapshot() const
{
    core::LockGuard<core::Mutex> lock(m_snapshotMutex);
    return m_snapshot;
}

bool PackFileSystem::TryFindAsset(AssetId assetId, AssetLocation& out) const
{
    auto snapshot = GetSnapshot();
    if (!snapshot)
    {
        return false;
    }

    for (const auto& pack : snapshot->packs)
    {
        auto it = pack->assetIndex.find(assetId);
        if (it == pack->assetIndex.end())
        {
            continue;
        }

        const AssetRecord& record = pack->assets[it->second];
        out.packId = pack->id;
        out.pack = pack.get();
        out.record = &record;
        return true;
    }

    return false;
}

bool PackFileSystem::TryFindChunk(ChunkId chunkId, ChunkLocation& out) const
{
    auto snapshot = GetSnapshot();
    if (!snapshot)
    {
        return false;
    }

    for (const auto& pack : snapshot->packs)
    {
        auto it = pack->chunkIndex.find(chunkId);
        if (it == pack->chunkIndex.end())
        {
            continue;
        }

        const ChunkRecord& record = pack->chunks[it->second];
        out.packId = pack->id;
        out.record = &record;
        out.file = pack->dataFile;
        out.fileOffset = record.offset;
        out.compressedSize = record.compressedSize;
        out.uncompressedSize = record.uncompressedSize;
        out.compression = record.compression;
        return true;
    }

    return false;
}

bool PackFileSystem::TryFindChunkInPack(PackId packId, ChunkId chunkId, ChunkLocation& out) const
{
    auto snapshot = GetSnapshot();
    if (!snapshot)
    {
        return false;
    }

    for (const auto& pack : snapshot->packs)
    {
        if (!pack || pack->id != packId)
        {
            continue;
        }

        auto it = pack->chunkIndex.find(chunkId);
        if (it == pack->chunkIndex.end())
        {
            return false;
        }

        const uint32_t index = it->second;
        if (index >= pack->chunks.size())
        {
            return false;
        }

        const ChunkRecord& record = pack->chunks[index];
        out.packId = pack->id;
        out.priority = pack->priority;
        out.record = &record;
        out.file = pack->dataFile;
        out.fileOffset = record.offset;
        out.compressedSize = record.compressedSize;
        out.uncompressedSize = record.uncompressedSize;
        out.compression = record.compression;
        return true;
    }

    return false;
}

bool PackFileSystem::ReadChunkSync(ChunkId chunkId, void* dst, size_t dstSize) const
{
    if (dstSize > std::numeric_limits<uint32_t>::max())
    {
        return false;
    }

    ChunkLocation location;
    if (!TryFindChunk(chunkId, location))
    {
        return false;
    }

    return ReadChunkSync(location, dst, static_cast<uint32_t>(dstSize));
}

bool PackFileSystem::ReadChunkSync(const ChunkLocation& location, void* dst, uint32_t dstSize) const
{
    if (!location.file || !location.file->IsValid())
    {
        return false;
    }

    if (dst == nullptr)
    {
        return false;
    }

    if (dstSize < location.compressedSize)
    {
        return false;
    }

    return location.file->ReadAt(location.fileOffset, dst, location.compressedSize);
}

PackReadHandle PackFileSystem::ReadChunkAsync(const ChunkLocation& location, void* dst, uint32_t dstSize, core::TaskSystem* taskSystem)
{
    if (!location.file || !location.file->IsValid())
    {
        return {};
    }

    if (dst == nullptr)
    {
        return {};
    }

    if (dstSize < location.compressedSize)
    {
        return {};
    }

    IORequest request = {};
    request.file = location.file;
    request.offset = location.fileOffset;
    request.size = location.compressedSize;
    request.dst = dst;
    request.dstSize = dstSize;
    return m_asyncIO.ReadAsync(request, taskSystem);
}

uint32_t PackFileSystem::GetMountedPackCount() const
{
    auto snapshot = GetSnapshot();
    if (!snapshot)
    {
        return 0;
    }

    return static_cast<uint32_t>(snapshot->packs.size());
}

bool PackFileSystem::LoadToc(core::StringView path, MountedPack& out)
{
    core::FileHandlePtr tocFile = core::FileSystem::Open({.path = path, .async = false, .sequential = true, .randomAccess = true,});
    if (!tocFile || !tocFile->IsValid())
    {
        return false;
    }

    const uint64_t tocFileSize = tocFile->GetSize();

    PackTocHeader header {};
    if (!tocFile->ReadAt(0, &header, sizeof(header)))
    {
        return false;
    }

    if (!ValidateHeader(header, tocFileSize))
    {
        return false;
    }

    out.tocHeader = header;

    if (!ReadTable(tocFile, header.assetTableOffset, header.assetCount, header.assetRecordSize, out.assets))
    {
        return false;
    }

    if (!ReadTable(tocFile, header.chunkTableOffset, header.chunkCount, header.chunkRecordSize, out.chunks))
    {
        return false;
    }

    if (!ReadTable(tocFile, header.dependencyTableOffset, header.dependencyCount, header.dependencyRecordSize, out.dependencies))
    {
        return false;
    }

    if (header.stringTableSize > 0)
    {
        out.stringTable.resize(header.stringTableSize);

        if (!tocFile->ReadAt(header.stringTableOffset, out.stringTable.data(), header.stringTableSize))
        {
            return false;
        }
    }
    else
    {
        out.stringTable.clear();
    }

    return true;
}

bool PackFileSystem::ValidateHeader(const PackTocHeader& header, uint64_t tocFileSize) const
{
    if (!IsValidMagic(header.magic))
    {
        return false;
    }

    if (header.version == 0 || header.version > s_PackFormatVersion)
    {
        return false;
    }

    if (header.assetRecordSize != sizeof(AssetRecord))
    {
        return false;
    }

    if (header.chunkRecordSize != sizeof(ChunkRecord))
    {
        return false;
    }

    if (header.dependencyRecordSize != sizeof(DependencyRecord))
    {
        return false;
    }

    if (header.alignment == 0)
    {
        return false;
    }

    if (header.dataFileSize == 0)
    {
        return false;
    }

    return true;
}

bool PackFileSystem::BuildIndex(MountedPack& pack)
{
    pack.assetIndex.clear();
    pack.chunkIndex.clear();

    pack.assetIndex.reserve(pack.assets.size());
    pack.chunkIndex.reserve(pack.chunks.size());

    for (uint32_t i = 0; i < pack.assets.size(); ++i)
    {
        const AssetRecord& record = pack.assets[i];

        if (record.assetId == s_InvalidAssetId)
        {
            return false;
        }

        if (record.metadataChunkId == s_InvalidChunkId)
        {
            return false;
        }

        auto [_, inserted] = pack.assetIndex.emplace(record.assetId, i);

        if (!inserted)
        {
            return false;
        }
    }

    for (uint32_t i = 0; i < pack.chunks.size(); ++i)
    {
        const ChunkRecord& record = pack.chunks[i];

        if (record.chunkId == s_InvalidChunkId)
        {
            return false;
        }

        if (record.compressedSize == 0)
        {
            return false;
        }

        if (record.uncompressedSize == 0)
        {
            return false;
        }

        uint64_t chunkEnd = record.offset + record.compressedSize;
        if (chunkEnd > pack.tocHeader.dataFileSize)
        {
            return false;
        }

        auto [_, inserted] = pack.chunkIndex.emplace(record.chunkId,i);
        if (!inserted)
        {
            return false;
        }
    }

    return true;
}

void PackFileSystem::SortByPriority(core::TVector<MountedPackPtr>& packs)
{
    std::sort(packs.begin(), packs.end(), [](const auto& a, const auto& b) {
        return a->priority > b->priority;
    });
}
}
