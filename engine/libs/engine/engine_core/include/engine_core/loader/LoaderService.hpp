#pragma once

#include "core/service/ServiceProvider.hpp"
#include "core/task/TaskSystem.hpp"
#include "core/pack/PackFilePool.hpp"
#include "engine_core/asset/AssetPool.hpp"

namespace slug::engine_core
{

class LoaderService : core::IService
{
public:
    SLUG_SERVICE_REGISTER_DEPENDENTS(slug::engine_core::LoaderService, core::TaskSystem)

    LoaderService(core::TaskSystem* taskSystem);
    void LoadPack(core::StringView filePath);

private:
    core::TaskSystemPtr m_taskSystem = nullptr;
    core::PackFilePoolPtr m_packFilePool = nullptr;
    AssetPoolPtr m_assetPool = nullptr;
};

}
