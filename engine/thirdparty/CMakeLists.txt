# googletest
if(SLUG_BUILD_TEST)
    add_subdirectory(googletest)
    set_target_properties(gmock PROPERTIES FOLDER thirdparty/gtest/)
    set_target_properties(gmock_main PROPERTIES FOLDER thirdparty/gtest/)
    set_target_properties(gtest PROPERTIES FOLDER thirdparty/gtest/)
    set_target_properties(gtest_main PROPERTIES FOLDER thirdparty/gtest/)
    setup_options(gmock)
    setup_options(gmock_main)
    setup_options(gtest)
    setup_options(gtest_main)
endif()

# google_benchmark
if(SLUG_BUILD_TEST)
    option(BENCHMARK_ENABLE_TESTING OFF)
    option(BENCHMARK_ENABLE_EXCEPTIONS OFF)
    option(BENCHMARK_ENABLE_WERROR OFF)
    add_subdirectory(google_benchmark)
    set_target_properties(benchmark PROPERTIES FOLDER thirdparty/benchmark/)
    set_target_properties(benchmark_main PROPERTIES FOLDER thirdparty/benchmark/)
    setup_options(benchmark)
    setup_options(benchmark_main)
endif()

#glm
if(SLUG_BUILD_TEST)
    add_subdirectory(glm)
    set_target_properties(glm PROPERTIES FOLDER thirdparty/glm/)
    setup_options(glm)
endif()

#slang
if(SLUG_BUILD_SHADER)
    execute_process(COMMAND cmd /c "${CMAKE_CURRENT_LIST_DIR}/slang/Install.bat" 
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/slang/" 
    RESULT_VARIABLE result)
    include(slang/slang.cmake)
endif()

# mimalloc
if(SLUG_USE_MIMALLOC)
    set(MI_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(MI_BUILD_STATIC ON CACHE BOOL "" FORCE)
    set(MI_BUILD_OBJECT OFF CACHE BOOL "" FORCE)
    set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(mimalloc)
    set_target_properties(mimalloc-static PROPERTIES FOLDER thirdparty/mimalloc)
    setup_options(mimalloc-static)
endif()