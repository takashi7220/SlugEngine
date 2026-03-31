# googletest
if(${SLUG_USE_GOOGLETEST})
    add_subdirectory(googletest)
    set_target_properties(gmock PROPERTIES FOLDER thirdparty/gtest/)
    set_target_properties(gmock_main PROPERTIES FOLDER thirdparty/gtest/)
    set_target_properties(gtest PROPERTIES FOLDER thirdparty/gtest/)
    set_target_properties(gtest_main PROPERTIES FOLDER thirdparty/gtest/)
endif()

# mimalloc
if(${SLUG_USE_MIMALLOC})
    set(MI_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(MI_BUILD_STATIC ON CACHE BOOL "" FORCE)
    set(MI_BUILD_OBJECT OFF CACHE BOOL "" FORCE)
    set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(mimalloc)
    set_target_properties(mimalloc-static PROPERTIES FOLDER thirdparty/mimalloc)
endif()