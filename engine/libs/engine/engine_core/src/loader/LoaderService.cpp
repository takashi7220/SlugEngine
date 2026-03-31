#include "engine_core/loader/LoaderService.hpp"
#include "engine_core/asset/AssetLoaderBase.hpp"

namespace slug
{
namespace engine_core
{

LoaderService::LoaderService(core::TaskSystem* taskSystem)
    : m_taskSystem(taskSystem)
    , m_packFilePool(nullptr)
    , m_assetPool(nullptr)
{
    m_packFilePool = new core::PackFilePool();
    m_assetPool = new AssetPool();
}

void LoaderService::LoadPack(core::StringView filePath)
{
    core::PackFilePath str = filePath;
    if (m_packFilePool->Contain(str))
    {
        return;
    }

    core::PackFilePtr packFile(new core::PackFile(filePath));
    m_packFilePool->Add(str, packFile);

    AssetLoaderBase assetLoader = {};
    assetLoader.Load(*m_taskSystem, *packFile, m_assetPool);
}

}
}
