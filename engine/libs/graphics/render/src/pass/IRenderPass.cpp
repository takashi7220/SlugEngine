#include "render/pass/IRenderPass.hpp"
#include "render/context/DrawContext.hpp"
#include "render/context/BindingCache.hpp"

namespace slug::render
{

void IRenderPass::Initialize(const InitializeParam& param)
{
    m_param = param;
}

}
