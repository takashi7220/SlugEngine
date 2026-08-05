set(GOOGLETEST_ROOT "${CMAKE_CURRENT_LIST_DIR}/.tmp/install")
set(GOOGLETEST_INCLUDE_DIR "${GOOGLETEST_ROOT}/Release/include")
set(GOOGLETEST_DEBUG_LIB_DIR "${GOOGLETEST_ROOT}/Debug/lib")
set(GOOGLETEST_RELEASE_LIB_DIR "${GOOGLETEST_ROOT}/Release/lib")

if(NOT EXISTS "${GOOGLETEST_INCLUDE_DIR}/gtest/gtest.h")
    message(FATAL_ERROR "GoogleTest is not installed. Run ${CMAKE_CURRENT_LIST_DIR}/Install.bat first.")
endif()

function(slug_import_googletest_target target debug_library release_library)
    if(NOT TARGET ${target})
        add_library(${target} STATIC IMPORTED GLOBAL)
        set_target_properties(${target} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GOOGLETEST_INCLUDE_DIR}"
            IMPORTED_CONFIGURATIONS "Debug;Release"
            IMPORTED_LOCATION_DEBUG "${debug_library}"
            IMPORTED_LOCATION_RELEASE "${release_library}"
            MAP_IMPORTED_CONFIG_OPTIMIZE Release
            MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
            MAP_IMPORTED_CONFIG_MINSIZEREL Release
        )
    endif()
endfunction()

slug_import_googletest_target(gtest
    "${GOOGLETEST_DEBUG_LIB_DIR}/gtest.lib"
    "${GOOGLETEST_RELEASE_LIB_DIR}/gtest.lib"
)
slug_import_googletest_target(gtest_main
    "${GOOGLETEST_DEBUG_LIB_DIR}/gtest_main.lib"
    "${GOOGLETEST_RELEASE_LIB_DIR}/gtest_main.lib"
)
slug_import_googletest_target(gmock
    "${GOOGLETEST_DEBUG_LIB_DIR}/gmock.lib"
    "${GOOGLETEST_RELEASE_LIB_DIR}/gmock.lib"
)
slug_import_googletest_target(gmock_main
    "${GOOGLETEST_DEBUG_LIB_DIR}/gmock_main.lib"
    "${GOOGLETEST_RELEASE_LIB_DIR}/gmock_main.lib"
)

set_target_properties(gtest_main PROPERTIES INTERFACE_LINK_LIBRARIES gtest)
set_target_properties(gmock PROPERTIES INTERFACE_LINK_LIBRARIES gtest)
set_target_properties(gmock_main PROPERTIES INTERFACE_LINK_LIBRARIES gmock)

if(NOT TARGET GTest::gtest)
    add_library(GTest::gtest ALIAS gtest)
endif()
if(NOT TARGET GTest::gtest_main)
    add_library(GTest::gtest_main ALIAS gtest_main)
endif()
if(NOT TARGET GTest::gmock)
    add_library(GTest::gmock ALIAS gmock)
endif()
if(NOT TARGET GTest::gmock_main)
    add_library(GTest::gmock_main ALIAS gmock_main)
endif()
