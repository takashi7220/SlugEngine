#include "resource/pack/PackFileSystem.hpp"
#include "core/filesystem/FileSystem.hpp"
#include <algorithm>

namespace slug::resource
{
#if 0
PackFileSystem::PackFileSystem()
{
    SnapshotPtr snapshot = core::MakeReference<Snapshot>();
    m_snapshot.store(snapshot, core::MemoryOrderRelease);
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

    const uint64_t dataSize = pack->dataFile->GetSize();
    BuildIndex(*pack);

    while (true)
    {
        SnapshotPtr oldSnapshot = m_snapshot.load(core::MemoryOrderAcquire);
    }
    return true;
}

void PackFileSystem::Umount(PackId packId)
{
    for (auto& pack : m_packs)
    {
        if (pack->id != packId)
        {
            continue;
        }

        if (pack->activeReadCount == 0 && pack->referencedResourceCount == 0)
        {
            pack->dataFile.reset();
            pack->state = MountedPackState::Unmounted;
        }
        else
        {
            pack->state = MountedPackState::PendingUnmount;
        }

        break;
    }

    m_packs.erase(
        std::remove_if(
            m_packs.begin(),
            m_packs.end(),
            [](const auto& pack)
            {
                return pack->state == MountedPackState::Unmounted;
            }),
        m_packs.end());
}

void PackFileSystem::UnmountAll()
{

}

bool PackFileSystem::TryFindAsset(AssetId assetId, AssetLocation& out) const
{
    core::ScopedLock lock(m_mutex);
    for (const auto& pack : m_packs)
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
    core::ScopedLock lock(m_mutex);
    for (const auto& pack : m_packs)
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

bool PackFileSystem::ReadChunkSync(ChunkId chunkId, void* dst, size_t dstSize) const
{
    ChunkLocation location;
    if (!TryFindChunk(chunkId, location))
    {
        return false;
    }

    if (!location.file || !location.file->IsValid())
    {
        return false;
    }

    if (dstSize < location.compressedSize)
    {
        return false;
    }

    return location.file->ReadAt(location.fileOffset, dst, location.compressedSize);
}

bool PackFileSystem::LoadToc(core::StringView path, MountedPack& out)
{
    auto tocFile = core::FileSystem::Open({.path = path, .async = false, .randomAccess = true,});
    if (!tocFile || !tocFile->IsValid())
    {
        return false;
    }

    const uint64_t tocSize = tocFile->GetSize();

    PackTocHeader header {};
    if (!tocFile->ReadAt(0, &header, sizeof(header)))
    {
        return false;
    }

    if (!ValidateHeader(header, tocSize))
    {
        return false;
    }

    out.tocHeader = header;
    out.assets.resize(header.assetCount);
    if (!tocFile->ReadAt(header.assetTableOffset, out.assets.data(), header.assetCount * sizeof(AssetRecord)))
    {
        return false;
    }

    out.chunks.resize(header.chunkCount);
    if (!tocFile->ReadAt(header.chunkTableOffset, out.chunks.data(), header.chunkCount * sizeof(ChunkRecord)))
    {
        return false;
    }

    if (header.dependencyCount > 0)
    {
        out.dependencies.resize(header.dependencyCount);

        if (!tocFile->ReadAt(header.dependencyTableOffset, out.dependencies.data(), header.dependencyCount * sizeof(DependencyRecord)))
        {
            return false;
        }
    }

    if (header.stringTableSize > 0)
    {
        out.stringTable.resize(header.stringTableSize);

        if (!tocFile->ReadAt(header.stringTableOffset, out.stringTable.data(), header.stringTableSize))
        {
            return false;
        }
    }

    return true;
}

bool PackFileSystem::ValidateHeader(const PackTocHeader& header, uint64_t tocFileSize) const
{

}

void PackFileSystem::BuildIndex(MountedPack& pack)
{
    pack.assetIndex.reserve(pack.assets.size());
    pack.chunkIndex.reserve(pack.chunks.size());

    for (uint32_t i = 0; i < pack.assets.size(); ++i)
    {
        pack.assetIndex.emplace(pack.assets[i].assetId, i);
    }

    for (uint32_t i = 0; i < pack.chunks.size(); ++i)
    {
        pack.chunkIndex.emplace(pack.chunks[i].chunkId, i);
    }
}

void PackFileSystem::SortByPriority()
{
    std::sort(
        m_packs.begin(),
        m_packs.end(),
        [](const auto& a, const auto& b)
        {
            return a->priority > b->priority;
        });
}
#endif
}
