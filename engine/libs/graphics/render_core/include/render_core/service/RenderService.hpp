#pragma once

#include "rhi/Rhi.hpp"
#include "render_core/resource/ResourcePool.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::render_core
{
class RenderService : public core::IService
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(RenderService, core::IService)
    SLUG_SERVICE_CUSTOM_REGISTER(RenderService)

    struct Desc
    {
        bool enableDebug = false;
        GraphicsApiType apiType = GraphicsApiType::D3D12;
    };

    struct WindowParam
    {
        core::StringView name;
        size_t width = 0;
        size_t height = 0;
        void* handle;
    };

    enum class CommandListType
    {
        Graphics,
        Compute,
        Resource,
        Copy
    };

public:
    RenderService(const Desc& desc);

    void RegisterWindow(const WindowParam& param);

    rhi::DevicePtr GetDevice() const;

    ResourcePool& GetResourcePool();

    const rhi::CommandListPtr& GetCommandList(CommandListType type) const;

    void Present(core::StringView name, uint32_t syncInterval);

    void Terminate();
private:
    rhi::IContextPtr m_context = nullptr;
    ResourcePool m_resourcePool = {};
    core::TVector<rhi::CommandListPtr> m_commandLists = {};
};
using RenderServicePtr = core::TReferencePtr<RenderService>;

}
