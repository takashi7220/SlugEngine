#pragma once
#include "core/memory/ReferencePtr.hpp"
#include "render/IGrapchicsDevice.hpp"

namespace slug::render
{

enum class GraphicsApiType
{
    Auto,
    D3D11,
    D3D12,
    Vulkan
};

struct GraphicsParameter : public GraphicsDeviceParameter
{
    GraphicsApiType graphicsApiType = GraphicsApiType::D3D12;
};

class GraphicsContext
{
public:
    static GraphicsContext* Create(GraphicsParameter parameter);

private:
    GraphicsContext() = default;
    virtual ~GraphicsContext() = default;
    GraphicsContext(const GraphicsContext& context) = delete;
    GraphicsContext operator=(const GraphicsContext& context) = delete;

private:
    GraphicsParameter m_parameter;
    core::TReferencePtr<IGraphicsDevice> m_device;
};

}
