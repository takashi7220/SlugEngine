#pragma once

#include "core/memory/ReferencePtr.hpp"
#include "render/Window.hpp"

namespace slug
{
namespace render
{

enum class GPUType
{
    Nvidia,
    Amd,
    Intel,
    Other,
};

struct GraphicsDeviceParameter
{
    core::TReferencePtr<IWindow> window = nullptr;
    uint32_t width = 1280;
    uint32_t height = 720;
    bool enableDebug = false;
};

class IGraphicsDevice : public core::SObject
{
public:
    virtual bool Create(const GraphicsDeviceParameter& param) = 0;
    virtual bool Destroy() = 0;
    virtual bool Resize(uint32_t width, uint32_t height) = 0;
    virtual bool GetCurrentBackBuffer() = 0;
    virtual uint32_t GetCurrentBackBufferIndex() = 0;
    virtual uint32_t GetBackBufferCount() = 0;
    virtual void Present(uint32_t syncInterval) = 0;
    virtual const char* GetAdapterName() = 0;
    virtual GPUType GetGPUType() = 0;
};

class GraphicsDeviceGenerator
{
public:
    static core::TReferencePtr<IGraphicsDevice> GenerateD3D12(GraphicsDeviceParameter param);
};

}
}
