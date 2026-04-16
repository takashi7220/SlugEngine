#pragma once

#include <iostream>
#include "core/debug/Logger.hpp"
#include "core/container/String.hpp"

#if DEBUG_MODE
#define SLUG_ASSERT(expression)                                                                                \
    if(!(expression))                                                                                          \
    {                                                                                                          \
        slug::core::StringStream __tmp_ss = {};                                                                \
        __tmp_ss << "file " << __FILE__ << ",line " << __LINE__ << "\n";                                       \
        slug::core::LogCategoryBase __def_category = {};                                                       \
        slug::core::Logger::Write(slug::core::LogType::Assert, &__def_category, __tmp_ss.str().c_str()); \
    }                                                                                                          \

#define SLUG_ASSERT_MSG(expression, msg)                                                                       \
    if(!(expression))                                                                                          \
    {                                                                                                          \
        slug::core::StringStream __tmp_ss = {};                                                                \
        __tmp_ss << #msg << ",file " << __FILE__ << ",line " << __LINE__ << "\n";                              \
        slug::core::LogCategoryBase __def_category = {};                                                       \
        slug::core::Logger::Write(slug::core::LogType::Assert, &__def_category, __tmp_ss.str().c_str()); \
    }                                                                                                          \

#define SLUG_ASSERT_RETURN(expression)                                                                         \
    if(!(expression))                                                                                          \
    {                                                                                                          \
        slug::core::StringStream __tmp_ss = {};                                                                \
        __tmp_ss << "file " << __FILE__ << ",line " << __LINE__ << "\n";                                       \
        slug::core::LogCategoryBase __def_category = {};                                                       \
        slug::core::Logger::Write(slug::core::LogType::Assert, &__def_category, __tmp_ss.str().c_str()); \
    }                                                                                                          \
    return #expression                                                                                         \

#define SLUG_ASSERT_MSG_RETURN(expression, msg)                                                                \
    if(!(expression))                                                                                          \
    {                                                                                                          \
        slug::core::StringStream __tmp_ss = {};                                                                \
        __tmp_ss << #msg << ",file " << __FILE__ << ",line " << __LINE__ << "\n";                              \
        slug::core::LogCategoryBase __def_category = {};                                                       \
        slug::core::Logger::Write(slug::core::LogType::Assert, &__def_category, __tmp_ss.str().c_str()); \
    }                                                                                                          \
    return #expression                                                                                         \

#define SLUG_LOG_SYSTEM(logType, msg)                                                                          \
{                                                                                                              \
    slug::core::StringStream __tmp_ss = {};                                                                    \
    __tmp_ss << msg << ",file " << __FILE__ << ",line " << __LINE__ << "\n";                                   \
    slug::core::debug::AssertUtility::WriteLog<slug::core::LogCategoryBase>(__tmp_ss.str().c_str(), logType);  \
}                                                                                                              \

#define SLUG_LOG(category, logType, msg)                                                    \
{                                                                                           \
    slug::core::StringStream __tmp_ss = {};                                                 \
    __tmp_ss << msg << ",file " << __FILE__ << ",line " << __LINE__ << "\n";                \
    slug::core::debug::AssertUtility::WriteLog<category>(__tmp_ss.str().c_str(), logType);  \
}                                                                                           \

#define SLUG_STATIC_ASSERT(expression) static_assert(expression)
#define SLUG_STATIC_ASSERT_MSG(expression, msg) static_assert(expression, msg)

namespace slug::core::debug
{

class AssertUtility
{
public:
    template<typename Category>
    static void WriteLog(core::StringView msg, LogType type)
    {
        Category category;
        slug::core::Logger::Write(type, &category, msg);
    }

};

}

#else
#define SLUG_ASSERT(expression)
#define SLUG_ASSERT_MSG(expression, msg)

#define SLUG_ASSERT_RETURN(expression)
#define SLUG_ASSERT_MSG_RETURN(expression, msg)

#define SLUG_STATIC_ASSERT(expression)
#define SLUG_STATIC_ASSERT_MSG(expression, msg)

#define SLUG_LOG_SYSTEM(logType, msg)
#define SLUG_LOG(category, logType, msg)
#endif
