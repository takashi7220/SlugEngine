#pragma once

#include "core/container/IntrusiveLinkedList.hpp"
#include "core/container/String.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

class ICommand;
class CommandService;

class CommandUtility
{
public:
    struct CommandRegistry
    {
        core::StringView name;
        core::TReferencePtr<ICommand>(*factory)();
    };

    using LinkedList = core::IntrusiveLinkedList<CommandRegistry>;
    using Node = LinkedList::Node;

    static void RegisterCommand(Node& node);
    static LinkedList& GetRegisteredCommands();
    static void RegisterAllCommands(CommandService& commandService);
};
}

// クラス定義の後、.cpp ファイル内で使用する
#define SLUG_AUTO_REGISTER_COMMAND(commandClass)                                                                          \
    namespace {                                                                                                           \
    static slug::core::CommandUtility::CommandRegistry g_commandRegistery_##commandClass {                                \
        #commandClass,                                                                                                    \
        []() -> slug::core::TReferencePtr<slug::core::ICommand> { return slug::core::MakeReference<commandClass>(); }     \
    };                                                                                                                    \
    static slug::core::CommandUtility::Node g_commandRegisteryNode_##commandClass {                                       \
        nullptr, nullptr, &g_commandRegistery_##commandClass                                                              \
    };                                                                                                                    \
    static bool g_commandRegisteryNode_##commandClass##_registered =                                                      \
        (slug::core::CommandUtility::RegisterCommand(g_commandRegisteryNode_##commandClass), true);                       \
    }
