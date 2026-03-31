#include "render/context/DrawContext.hpp"

namespace slug
{
namespace render
{

DrawContext* DrawContext::Create(const SetupParam& param)
{
    DrawContext* context = new DrawContext();
    if (param.apiType == GraphicsApiType::D3D12)
    {
        rhi::IContext::SetupParam rhiParam = {};
        rhiParam.enableDebug = param.enableDebug;
        context->m_context = rhi::ContextGenerator::GenerateD3D12(rhiParam);
    }

    context->m_resourceService = ResourceService::Create();
    context->m_frameGraph = FrameGraph::Create();
    return context;
}

void DrawContext::AddWindow(const WindowParam& param)
{
    rhi::IContext::WindowParam windowParam;
    windowParam.name = param.name;
    windowParam.width = param.width;
    windowParam.height = param.height;
    windowParam.handle = param.handle;
    m_context->AddWindow(windowParam);
}

rhi::DevicePtr DrawContext::GetDevice() const
{
    return m_context->GetDevice();
}

ResourceServicePtr& DrawContext::GetResourceService()
{
    return m_resourceService;
}

const rhi::CommandListPtr& DrawContext::GetCreateResourceCommand() const
{
    return m_commandLists.at(0);
}

void DrawContext::Present(core::StringView name, uint32_t syncInterval)
{
    m_context->Present(name, syncInterval);
}

}
}
