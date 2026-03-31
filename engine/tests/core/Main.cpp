#include <gtest/gtest.h>
#include "core/debug/Logger.hpp"
#include <mimalloc.h>
#include <mimalloc-new-delete.h>
#include "core/memory/Allocator.hpp"

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

static void MemoryLeakCheckFunc(size_t leakMemorySize, const char* label)
{
    printf("Error : memory leak detected. %s : %llu\n", label, leakMemorySize);
    EXPECT_EQ(leakMemorySize, 0);
}

int main(int argc, char** argv) {
    int ret = 0;
    core::MemoryCounter counter;
    {
        try
        {
            core::MemoryUtilities::SetMemoryCounter(&counter);
            core::MemoryUtilities::SetMemoryLeackCheckCallback(MemoryLeakCheckFunc);

            TestLogController logController;
            core::Logger::BindController(&logController);

            mi_option_enable(mi_option_verbose);
            mi_option_enable(mi_option_show_errors);

            {
                SLUG_MEMORY_LEACK_CHECK_SCOPE(core_test)
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

    counter.Print();
    if (counter.GetCurrentMemorySize() != 0)
    {
        ret = 1;
    }
    return ret;
}
