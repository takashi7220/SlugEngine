#pragma once

#include "core/service/ServiceProvider.hpp"
#include "core/command/CommandParser.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

class ICommand : public core::ReferenceObject
{
public:
    virtual bool Execute(ServiceProvider& serviceProvider, const core::TVector<core::String>& args) = 0;
    virtual bool Execute(ServiceProvider& serviceProvider, int argc, char** argv) = 0;
    virtual void OnError(const CommandParser::ParseResult& result) = 0;
};

template<typename TCommandOptions>
class CommandBase : public ICommand
{
public:
    bool Execute(ServiceProvider& serviceProvider,const core::TVector<core::String>& args) override
    {
        TCommandOptions options = {};
        TCommandParser<TCommandOptions> parser(options);
        CommandParser::ParseResult result = parser.Parse(args);
        if (result.resultType == CommandParser::ResultType::Success)
        {
            return Execute(serviceProvider, options);
        }
        else
        {
            OnError(result);
            return false;
        }
    }

    bool Execute(ServiceProvider& serviceProvider, int argc, char** argv) override
    {
        TCommandOptions options = {};
        TCommandParser<TCommandOptions> parser(options);
        CommandParser::ParseResult result = parser.Parse(argc, argv);
        if (result.resultType == CommandParser::ResultType::Success)
        {
            return Execute(serviceProvider, options);
        }
        else
        {
            OnError(result);
            return false;
        }
    }

    virtual bool Execute(ServiceProvider& serviceProvider, const TCommandOptions& options) = 0;
};
}
