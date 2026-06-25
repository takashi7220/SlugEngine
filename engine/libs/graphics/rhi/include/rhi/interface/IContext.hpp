#pragma once

#include "core/memory/ReferencePtr.hpp"

#include "core/container/String.hpp"
#include "rhi/interface/IDevice.hpp"

namespace slug::rhi
{

enum class GPUType
{
    Nvidia,
    Amd,
    Intel,
    Other,
};

class IContext : public core::ReferenceObject
{
public:
    struct WindowParam
    {
        core::StringView name;
        size_t width = 0;
        size_t height = 0;
        void* handle;
    };

    struct SetupParam
    {
        bool enableDebug = false;
    };

public:
    virtual bool Create(const SetupParam& param) = 0;
    virtual bool Destroy() = 0;
    virtual rhi::DevicePtr GetDevice() = 0;

    virtual void AddWindow(WindowParam& param) = 0;
    virtual void RemoveWindow(core::StringView name) = 0;
    virtual size_t GetWindowCount() = 0;
    virtual bool HasWindow(core::StringView name) = 0;

    virtual bool Resize(core::StringView windowName, uint32_t width, uint32_t height) = 0;

    virtual TexturePtr GetCurrentBackBuffer(core::StringView windowName) = 0;
    virtual TexturePtr GetBackBuffer(core::StringView windowName, uint32_t index) = 0;
    virtual uint32_t GetCurrentBackBufferIndex(core::StringView windowName) = 0;
    virtual uint32_t GetBackBufferCount(core::StringView windowName) = 0;

    virtual void Present(core::StringView windowName, uint32_t syncInterval) = 0;
    virtual void PresentAll(uint32_t syncInterval) = 0;

    virtual core::StringView GetAdapterName() = 0;
    virtual GPUType GetGPUType() = 0;
};
using IContextPtr = core::TReferencePtr<IContext>;

class ContextGenerator
{
public:
    static IContextPtr GenerateD3D12(const IContext::SetupParam& param);
};

}
