#pragma once

#include "core/TypeConcept.hpp"
#include "core/pattern/Singleton.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/container/String.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

enum class LogType  : uint8_t
{
    Info = 0,
    Warning,
    Error,
    Assert,
    Alert,
    Fatal
};

inline const char* EnumrateLogTypeNames(LogType type) 
{
    switch (type) 
    {
    case LogType::Info:
        return "Info";
    case LogType::Warning:
        return "Warning";
    case LogType::Error:
        return "Error";
    case LogType::Assert:
        return "Assert";
    case LogType::Fatal:
        return "Fatal";
    default:
        return "Unknown";
    }
}

class ILogCategory
{
public:
    virtual core::StringView GetName() const = 0;
};
using LogCategoryPtr = core::TReferencePtr<ILogCategory>;
template<typename T> concept LogCategoryType = std::derived_from<T, ILogCategory>;

class ILoggerController : public core::ReferenceObject
{
public:
    ILoggerController() = default;
    virtual void Write(LogType type, ILogCategory* category, core::StringView message) = 0;
};

using LoggerControllerPtr = core::TReferencePtr<ILoggerController>;
template<typename T> concept LoggerControllerType = std::derived_from<T, ILoggerController>;

class LogCategoryBase : public core::ILogCategory
{
public:
    virtual core::StringView GetName() const override
    {
        return "System";
    }
};

class DefaultLoggerController : public ILoggerController
{
public:
    DefaultLoggerController();
    void Write(LogType type, ILogCategory* category, core::StringView message) override;
};

class Logger
{
public:
    static void Write(LogType type, ILogCategory* category, core::StringView message);
    static void BindController(ILoggerController* controller);
    static void UnbindController();
};

class Log 
{
public:
    template <std::size_t Size = 512, class... Args>
    static void Write(LogType type, ILogCategory* category, const char* fmt, Args&&... args)
    {
        String str = StringUtility::Sprintfs(fmt, std::forward<Args>(args)...);
        Logger::Write(type, category, str);
    }

    template<LogCategoryType T, class... Args>
    static void Write(LogType type, const char* fmt, Args&&... args)
    {
        
        T category = {};
        String str = StringUtility::Sprintfs(fmt, std::forward<Args>(args)...);
        Logger::Write(type, &category, str);
    }
};
}
