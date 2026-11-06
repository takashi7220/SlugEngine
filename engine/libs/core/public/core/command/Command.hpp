#pragma once

#include "core/service/ServiceProvider.hpp"
#include "core/command/CommandParser.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/pattern/StaticRegistry.hpp"

#define SLUG_COMMAND_ID(className) CommandID(SLUG_NAMEOF(className))

#define SLUG_DECLARE_COMMAND(className)                                                       \
    static CommandID GetStaticCommandID() {                                                   \
        return SLUG_COMMAND_ID(className);                                                    \
    }                                                                                         \
    static slug::core::ICommand* Create() {                                                   \
        return slug::core::NewObject<className>();                                            \
    }                                                                                         \
    static slug::core::CommandStaticRegistry::RegistryType* GetStaticRegistry() {             \
        static slug::core::CommandStaticRegistry::RegistryType registry(                      \
            className::GetStaticCommandID(),                                                  \
            &className::Create);                                                              \
        return &registry;                                                                     \
    }                                                                                         \
    static slug::core::CommandStaticRegistry::Node* GetStaticNode() {                         \
        static slug::core::CommandStaticRegistry::Node node(className::GetStaticRegistry());  \
        return &node;                                                                         \
    }                                                                                         \

#define SLUG_REGISTER_COMMAND(className)                                                      \
    static const bool s_registered_command_##className = []() {                               \
        slug::core::CommandStaticRegistry::RegisterNode(className::GetStaticNode());          \
        return true;                                                                          \
    }();                                                                                      \

namespace slug::core
{

using CommandID = core::FixedString<256>;

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

struct CommandRegistry
{
    CommandID id;
    ICommand* (*factory)();

    CommandRegistry(CommandID id, ICommand* (*factory)())
        : id(id), factory(factory)
    {
    }
};

class CommandStaticRegistry : public TStaticRegistry<CommandRegistry>
{
private:
    friend class CommandService;
};

}
