#include "core/debug/Logger.hpp"

namespace slug::core
{

DefaultLoggerController::DefaultLoggerController()
    : ILoggerController()
{
}

void DefaultLoggerController::Write(LogType type, ILogCategory* category, core::StringView message)
{
    core::StringStream ss;
    ss << "[type] : " << EnumrateLogTypeNames(type) << ",";
    if (category)
    {
        ss << "[category] : " << category->GetName() << ",";
    }
    ss << "[message] : " << message;
    printf("%s\n", ss.str().c_str());
}

void Logger::Write(LogType type, ILogCategory* category, core::StringView message)
{
    if (!GetRegistry().controller)
    {
        static DefaultLoggerController defaultController;
        GetRegistry().controller = &defaultController;
    }

    if (GetRegistry().controller)
    {
        GetRegistry().controller->Write(type, category, message);
    }
}

void Logger::BindController(ILoggerController* controller)
{
    GetRegistry().controller = controller;
}

Logger::Registry& Logger::GetRegistry()
{
    static Logger::Registry s_Registry = {};
    return s_Registry;
}

void Logger::UnbindController()
{
    GetRegistry().controller = nullptr;
}

}
