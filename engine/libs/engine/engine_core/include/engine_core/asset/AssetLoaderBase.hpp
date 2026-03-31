#pragma once

#include "core/task/TaskSystem.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/pack/PackFile.hpp"
#include "engine_core/asset/AssetPool.hpp"

namespace slug
{
namespace engine_core
{
class AssetLoaderBase
{
public:
    struct LoadParam
    {
        AssetHeader header = {};
        void* data = nullptr;
        size_t size = 0;
    };

    virtual core::TVector<AssetBasePtr> Load(core::TaskSystem& taskSystem, const core::PackFile& pack, const AssetPoolPtr& assetPool);
    virtual core::TVector<AssetBasePtr> Load(core::TaskSystem& taskSystem, const core::TVector<LoadParam>& params, const AssetPoolPtr& assetPool);
    virtual AssetBasePtr LoadInternal(const LoadParam& param);
private:

};

}
}
