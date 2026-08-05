#include "render_core/pass/IRenderPass.hpp"

namespace slug::render_core
{

void IRenderPass::Initialize(const InitializeParam& param)
{
    m_param = param;
}

}
