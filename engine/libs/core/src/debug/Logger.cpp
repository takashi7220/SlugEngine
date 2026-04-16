#include "core/debug/Logger.hpp"

namespace slug::core
{

static ILoggerController* s_LoggerController = nullptr;

void DefaultLoggerWrite(LogType type, ILogCategory* category, core::StringView message)
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

DefaultLoggerController::DefaultLoggerController()
    : ILoggerController()
{
}

void DefaultLoggerController::Write(LogType type, ILogCategory* category, core::StringView message)
{
    DefaultLoggerWrite(type, category, message);
}

void Logger::Write(LogType type, ILogCategory* category, core::StringView message)
{
    if (s_LoggerController)
    {
        s_LoggerController->Write(type, category, message);
    }
    else
    {
        DefaultLoggerWrite(type, category, message);
    }
}

void Logger::BindController(ILoggerController* controller)
{
    s_LoggerController = controller;
}

void Logger::UnbindController()
{
    s_LoggerController = nullptr;
}

}
