#include "core/command/CommandService.hpp"

namespace slug::core
{
CommandService::CommandService()
{
    CommandStaticRegistry::GetStaticRegisteredNodes().for_each([this](CommandRegistry* registry){
        RegisterCommand(registry->id, registry->factory());
    });
}

void CommandService::RegisterCommand(CommandID commandID, ICommand* command)
{
    m_commands[commandID] = command;
}

void CommandService::UnregisterCommand(CommandID commandID)
{
    m_commands.erase(commandID);
}

void CommandService::ClearCommands()
{
    m_commands.clear();
}

core::TReferencePtr<ICommand> CommandService::GetCommand(CommandID commandID) const
{
    auto it = m_commands.find(commandID);
    if (it != m_commands.end())
    {
        return it->second;
    }
    return nullptr;
}

void CommandService::GetCommandNames(core::TVector<core::String>& commandNames) const
{
    for (const auto& pair : m_commands)
    {
        commandNames.push_back(pair.first.c_str());
    }
}

bool CommandService::ExecuteCommand(ServiceProvider& serviceProvider, CommandID commandID, int argc, char** argv)
{
    auto command = GetCommand(commandID);
    if (command)
    {
        return command->Execute(serviceProvider, argc, argv);
    } else
    {
        return false;
    }
}

bool CommandService::ExecuteCommand(ServiceProvider& serviceProvider, core::StringView commandName, const core::TVector<core::String>& args)
{
    if (args.empty())
    {
        return false;
    }

    auto command = GetCommand(commandName);
    if (command)
    {
        return command->Execute(serviceProvider, args);
    } else
    {
        return false;
    }
}

}
