#include "core/command/CommandUtility.hpp"
#include "core/command/CommandService.hpp"

namespace slug::core
{
void CommandUtility::RegisterCommand(Node& node)
{
    GetRegisteredCommands().push_back(&node);
}

CommandUtility::LinkedList& CommandUtility::GetRegisteredCommands()
{
    static CommandUtility::LinkedList registeredCommands;
    return registeredCommands;
}
    
void CommandUtility::RegisterAllCommands(CommandService& commandService)
{
    auto& registeredCommands = GetRegisteredCommands();
    registeredCommands.for_each([&commandService](CommandRegistry* reg){
        commandService.RegisterCommand(reg->name, reg->factory());
    });
}
}
