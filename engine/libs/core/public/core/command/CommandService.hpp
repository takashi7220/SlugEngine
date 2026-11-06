#pragma once

#include "core/service/ServiceProvider.hpp"
#include "core/command/Command.hpp"
#include "core/container/UnorderedMap.hpp"

namespace slug::core
{
class CommandService : public core::IService
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(CommandService, core::IService);
    SLUG_SERVICE_REGISTER(CommandService);

    CommandService();

    void RegisterCommand(CommandID commandID, ICommand* command);
    void UnregisterCommand(CommandID commandID);
    void ClearCommands();

    core::TReferencePtr<ICommand> GetCommand(CommandID commandID) const;
    void GetCommandNames(core::TVector<core::String>& commandNames) const;

    bool ExecuteCommand(ServiceProvider& serviceProvider, CommandID commandID, int argc, char** argv);
    bool ExecuteCommand(ServiceProvider& serviceProvider, core::StringView commandName, const core::TVector<core::String>& args);
private:
    core::TUnorderedMap<CommandID, core::TReferencePtr<ICommand>, FixedStringHash> m_commands;
};
}
