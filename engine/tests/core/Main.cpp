#include <gtest/gtest.h>
#include "core/debug/Logger.hpp"
#include <mimalloc.h>
#include <mimalloc-new-delete.h>
#include "core/memory/Allocator.hpp"
#include "core/memory/debug/MemoryDebugUtilities.hpp"

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
#if defined(DEBUG_MODE)
    printf("Error : memory leak detected. %s : %llu\n", core::MemoryDebugUtilities::GetMemoryLabelName(label), leakMemorySize);
#endif
    EXPECT_EQ(leakMemorySize, 0);
}

int main(int argc, char** argv) {
    int ret = 0;
#if defined(DEBUG_MODE)
    core::MemoryCounter counter;
    core::MemoryDebugUtilities::SetMemoryCounter(&counter);
    core::MemoryDebugUtilities::SetMemoryLeackCheckCallback(MemoryLeakCheckFunc);
#endif
    {
        try
        {

            TestLogController logController;
            core::Logger::BindController(&logController);

            mi_option_enable(mi_option_verbose);
            mi_option_enable(mi_option_show_errors);
            {
                SLUG_MEMORY_LABEL_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug);
                SLUG_MEMORY_LEACK_CHECK_SCOPE(slug::core::MemoryLabelType::MemoryLabelType_Debug)
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
#if defined(DEBUG_MODE)
    if (counter.GetCurrentTotalMemorySize() != 0)
    {
        ret = 1;
    }
#endif
    return ret;
}
