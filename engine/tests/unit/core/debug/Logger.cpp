#include "core/debug/Logger.hpp"
#include <gtest/gtest.h>

using namespace slug;

class TestLogCategory : public core::LogCategoryBase
{
public:
    core::StringView GetName() const override
    {
        return "Test";
    }
};


TEST(LoggerTest, Write)
{
    core::Log::Write<TestLogCategory>(core::LogType::Info, "test");
}
