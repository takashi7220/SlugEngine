#pragma once

#include "rhi/ResourceType.hpp"
namespace slug::rhi
{
class IMessageCallback 
{
protected:
    IMessageCallback() = default;
    virtual ~IMessageCallback() = default;

public:
    virtual void Message(MessageSeverity severity, const char* message_text) = 0;

    IMessageCallback(const IMessageCallback&) = delete;
    IMessageCallback(const IMessageCallback&&) = delete;
    IMessageCallback& operator=(const IMessageCallback&) = delete;
    IMessageCallback& operator=(const IMessageCallback&&) = delete;
};
}