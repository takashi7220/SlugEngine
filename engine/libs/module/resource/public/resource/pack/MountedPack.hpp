#pragma once

#include "resource/pack/PackHeader.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/filesystem/IFileHandle.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::resource
{

using FilePath = core::FixedString<260>;

struct MountDesc
{
    FilePath tocPath;
    FilePath dataPath;
    int32_t priority = 0;
    bool validateHash = false;
};

enum class MountedPackState : uint8_t
{
    Mounted,
    PendingUnmount,
    Unmounted,
};

class MountedPack;
using MountedPackPtr = core::TReferencePtr<MountedPack>;

class MountedPack : public core::ReferenceObject
{
public:
    PackId id = s_InvalidPackId;

    FilePath tocPath;
    FilePath dataPath;

    int priority = 0;

    PackTocHeader tocHeader {};

    core::TVector<AssetRecord> assets;
    core::TVector<ChunkRecord> chunks;
    core::TVector<DependencyRecord> dependencies;
    core::TVector<char> stringTable;

    core::TUnorderedMap<AssetId, uint32_t> assetIndex;
    core::TUnorderedMap<ChunkId, uint32_t> chunkIndex;

    core::FileHandlePtr dataFile;

    MountedPackState state = MountedPackState::Mounted;
    uint32_t activeReadCount = 0;
    uint32_t referencedResourceCount = 0;
};
}
