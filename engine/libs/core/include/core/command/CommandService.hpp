#pragma once

#include "core/command/Command.hpp"
#include "core/command/CommandUtility.hpp"
#include "core/container/UnorderedMap.hpp"

namespace slug::core
{
class CommandService : public core::IService
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(CommandService, core::IService)
    SLUG_SERVICE_REGISTER(CommandService)

    using CommandKey = core::FixedString<256>;

    CommandService()
    {
        CommandUtility::RegisterAllCommands(*this);
    }

    void RegisterCommand(core::StringView commandName, core::TReferencePtr<ICommand> command)
    {
        m_commands[commandName] = command;
    }

    void UnregisterCommand(core::StringView commandName)
    {
        m_commands.erase(commandName);
    }

    void ClearCommands()
    {
        m_commands.clear();
    }

    core::TReferencePtr<ICommand> GetCommand(core::StringView commandName) const
    {
        auto it = m_commands.find(commandName);
        if (it != m_commands.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void GetRegisteredCommandNames(core::TVector<core::String>& commandNames) const
    {
        for (const auto& pair : m_commands)
        {
            commandNames.push_back(pair.first.c_str());
        }
    }

    bool ExecuteCommand(ServiceProvider& serviceProvider, core::StringView commandName, int argc, char** argv)
    {
        auto command = GetCommand(commandName);
        if (command)
        {
            return command->Execute(serviceProvider, argc, argv);
        }
        else
        {
            return false;
        }
    }

    bool ExecuteCommand(ServiceProvider& serviceProvider, core::StringView commandName, const core::TVector<core::String>& args)
    {
        if (args.empty())
        {
            return false;
        }

        auto command = GetCommand(commandName);
        if (command)
        {
            return command->Execute(serviceProvider, args);
        }
        else
        {
            return false;
        }
    }
public:
    template<typename TCommand>
    void RegisterCommand()
    {
        static_assert(std::is_base_of<ICommand, TCommand>::value, "TCommand must derive from ICommand");
        core::TReferencePtr<ICommand> command = core::MakeReference<TCommand>();
        RegisterCommand(TCommand::GetCommandName(), command);
    }

private:
    core::TUnorderedMap<CommandKey, core::TReferencePtr<ICommand>, FixedStringHash> m_commands;
};
}
