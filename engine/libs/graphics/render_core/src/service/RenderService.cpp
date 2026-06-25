#include "render_core/service/RenderService.hpp"
#include "core/debug/CompileMessage.hpp"

namespace slug::render_core
{

RenderService::RenderService(const Desc& desc)
{
    rhi::IContext::SetupParam rhiParam = {};
    rhiParam.enableDebug = desc.enableDebug;

    if (desc.apiType == GraphicsApiType::D3D12)
    {
        m_context = rhi::ContextGenerator::GenerateD3D12(rhiParam);
    }
}

void RenderService::RegisterWindow(const WindowParam& param)
{
    rhi::IContext::WindowParam windowParam;
    windowParam.name = param.name;
    windowParam.width = param.width;
    windowParam.height = param.height;
    windowParam.handle = param.handle;
    m_context->AddWindow(windowParam);
}

rhi::DevicePtr RenderService::GetDevice() const
{
    return m_context->GetDevice();
}

ResourcePool& RenderService::GetResourcePool()
{
    return m_resourcePool;
}

const rhi::CommandListPtr& RenderService::GetCommandList(CommandListType type) const
{
    SLUG_COMPILE_TODO_MESSAGE("provide adaptive command list")
    return m_commandLists.at(0);
}

void RenderService::Present(core::StringView name, uint32_t syncInterval)
{
    m_context->Present(name, syncInterval);
}

void RenderService::Terminate()
{
    m_context.reset();
}
}
