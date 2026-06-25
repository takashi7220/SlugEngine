#pragma once

#include "resource/pack/PackLocation.hpp"
#include "core/thread/Atomic.hpp"

namespace slug::resource
{

class PackFileSystem
{
public:
    PackFileSystem();
    ~PackFileSystem();

    PackFileSystem(const PackFileSystem&) = delete;
    PackFileSystem& operator=(const PackFileSystem&) = delete;

    bool Mount(const MountDesc& desc);
    void Umount(PackId packId);
    void UnmountAll();

    bool TryFindAsset(AssetId assetId, AssetLocation& out) const;
    bool TryFindChunk(ChunkId chunkId, ChunkLocation& out) const;
    bool TryFindChunkInPack(PackId packId, ChunkId chunkId, ChunkLocation& out) const;

    bool ReadChunkSync(ChunkId chunkId, void* dst, size_t dstSize) const;
    bool ReadChunkSync(const ChunkLocation& location, void* dst, uint32_t dstSize) const;

    uint32_t GetMountedPackCount() const;
private:
    struct Snapshot : core::ReferenceObject
    {
        core::TVector<MountedPackPtr> packs = {};
    };
    using SnapshotPtr = core::TReferencePtr<Snapshot>;

    bool LoadToc(core::StringView path, MountedPack& out);
    bool ValidateHeader(const PackTocHeader& header, uint64_t tocFileSize) const;

    bool BuildIndex(MountedPack& pack);
    void SortByPriority(core::TVector<MountedPackPtr>& packs);

private:
    core::TAtomic<SnapshotPtr> m_snapshot;
    core::TAtomic<PackId> m_nextPackId = 0;
};
}
