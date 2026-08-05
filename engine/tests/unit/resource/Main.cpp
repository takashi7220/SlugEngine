#include <gtest/gtest.h>


int main(int argc, char** argv)
{
    int ret = 0;
    {
        try
        {
            testing::InitGoogleTest();
            ret = RUN_ALL_TESTS();
        } catch (std::runtime_error e)
        {
            printf("%s\n", e.what());
            ret = 1;
        }
    }
    return ret;
}
