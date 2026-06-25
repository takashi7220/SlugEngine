#include <gtest/gtest.h>
#include "core/debug/Logger.hpp"
#include "core/memory/Allocator.hpp"
#include "core/memory/debug/MemoryDebugUtility.hpp"

using namespace slug;

class TestLogController : public core::ILoggerController
{
public:
    TestLogController()
        : core::ILoggerController()
    {
    }

    void Write(core::LogType type, core::ILogCategory* category, core::StringView message) override
    {
        printf("[ LOGGER   ]");
        printf(" %s(%s) : %s", core::EnumrateLogTypeNames(type), category->GetName().data(), message.data());
        if (type == core::LogType::Assert)
        {
            __debugbreak();
        }
    }
};

void MemoryLeakCheckFunc(size_t leakMemorySize, core::MemoryLabel label)
{
#if SLUG_DEBUG_MODE
    printf("Error : memory leak detected. %s : %llu\n", label.name, leakMemorySize);
    EXPECT_EQ(leakMemorySize, 0);
#endif
}

int main(int argc, char** argv) {
    int ret = 0;
#if SLUG_DEBUG_MODE
    core::MemoryCounter counter;
    core::MemoryDebugUtility::BindMemoryCounter(&counter);
    core::MemoryDebugUtility::BindMemoryLeakCheckCallback(MemoryLeakCheckFunc);
#endif
    {
        try
        {

            TestLogController logController;
            core::Logger::BindController(&logController);
            core::MemoryUtility::EnableDebugLayer();
            {
                SLUG_MEMORY_LABEL_SCOPE(Debug);
                SLUG_MEMORY_LEACK_CHECK_SCOPE(Debug, Default)
                ::testing::InitGoogleTest(&argc, argv);
                ret = RUN_ALL_TESTS();
            }
        }
        catch (std::runtime_error e)
        {
            printf("%s\n", e.what());
            ret = 1;
        }
    }
#if SLUG_DEBUG_MODE
    EXPECT_EQ(counter.GetCurrentTotalMemorySize(), 0);
#endif
    EXPECT_EQ(core::MemoryUtility::GetCurrentProfile().malloc_requested_current, 0);
    return ret;
}
