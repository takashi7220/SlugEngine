set(MIMALLOC_ROOT "${CMAKE_CURRENT_LIST_DIR}/.tmp/install")
set(MIMALLOC_INCLUDE_DIR "${MIMALLOC_ROOT}/Release/include")
set(MIMALLOC_DEBUG_LIB "${MIMALLOC_ROOT}/Debug/lib/mimalloc-debug.lib")
set(MIMALLOC_RELEASE_LIB "${MIMALLOC_ROOT}/Release/lib/mimalloc.lib")

if(NOT EXISTS "${MIMALLOC_DEBUG_LIB}")
    set(MIMALLOC_DEBUG_LIB "${MIMALLOC_ROOT}/Debug/lib/mimalloc.lib")
endif()

if(NOT EXISTS "${MIMALLOC_INCLUDE_DIR}/mimalloc.h")
    message(FATAL_ERROR "mimalloc is not installed. Run ${CMAKE_CURRENT_LIST_DIR}/Install.bat first.")
endif()

if(NOT TARGET mimalloc-static)
    add_library(mimalloc-static STATIC IMPORTED GLOBAL)
    set_target_properties(mimalloc-static PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${MIMALLOC_INCLUDE_DIR}"
        IMPORTED_CONFIGURATIONS "Debug;Release"
        IMPORTED_LOCATION_DEBUG "${MIMALLOC_DEBUG_LIB}"
        IMPORTED_LOCATION_RELEASE "${MIMALLOC_RELEASE_LIB}"
        MAP_IMPORTED_CONFIG_OPTIMIZE Release
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
        MAP_IMPORTED_CONFIG_MINSIZEREL Release
    )
endif()

if(NOT TARGET mimalloc::mimalloc-static)
    add_library(mimalloc::mimalloc-static ALIAS mimalloc-static)
endif()
