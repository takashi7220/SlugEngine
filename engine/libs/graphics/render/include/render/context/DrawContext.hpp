#pragma once

#include "rhi/Rhi.hpp"
#include "render/resource/ResourceService.hpp"
#include "render/graph/FrameGraph.hpp"

namespace slug
{
namespace render
{


class DrawContext : public core::ReferenceObject
{
public:
    struct SetupParam
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
public:
    static DrawContext* Create(const SetupParam& param);

    void AddWindow(const WindowParam& param);
    rhi::DevicePtr GetDevice() const;
    ResourceServicePtr& GetResourceService();
    const rhi::CommandListPtr& GetCreateResourceCommand() const;
    void Present(core::StringView name, uint32_t syncInterval);

private:
    rhi::IContextPtr m_context = nullptr;
    FrameGraphPtr m_frameGraph = nullptr;
    ResourceServicePtr m_resourceService = nullptr;
    core::TVector<rhi::CommandListPtr> m_commandLists = {};
};
using DrawContextPtr = core::TReferencePtr<DrawContext>;

}
}
