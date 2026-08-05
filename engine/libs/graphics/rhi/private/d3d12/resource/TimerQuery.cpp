#include "rhi/d3d12/resource/TimerQuery.hpp"

namespace slug::rhi::d3d12
{

TimerQuery::TimerQuery(DeviceResources& resources)
    : m_resources(resources)
{
}


TimerQuery::~TimerQuery()
{
    m_resources.timerQueries.Release(static_cast<int32_t>(beginQueryIndex) / 2);
}

}
