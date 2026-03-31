#pragma once
#include "render/GraphicsContext.hpp"

namespace slug
{
namespace render
{

GraphicsContext* GraphicsContext::Create(GraphicsParameter parameter)
{
    GraphicsContext* context = new GraphicsContext();
    context->m_parameter = parameter;
    context->m_device = GraphicsDeviceGenerator::GenerateD3D12(parameter);
    return context;
}

}
}
