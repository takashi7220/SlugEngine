#pragma once
#include "rhi/Rhi.hpp"

namespace slug::render
{

class ScopedMarker
{
public:
    ScopedMarker(const rhi::CommandListPtr& commandList, core::StringView markerName)
        : m_commandList(commandList)
    {
        m_commandList->BeginMarker(markerName);
    }

    virtual ~ScopedMarker()
    {
        m_commandList->EndMarker();
    }
private:
    rhi::CommandListPtr m_commandList;
};

}
