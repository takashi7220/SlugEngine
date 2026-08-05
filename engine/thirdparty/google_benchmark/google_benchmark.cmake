set(GOOGLE_BENCHMARK_ROOT "${CMAKE_CURRENT_LIST_DIR}/.tmp/install")
set(GOOGLE_BENCHMARK_INCLUDE_DIR "${GOOGLE_BENCHMARK_ROOT}/Release/include")
set(GOOGLE_BENCHMARK_DEBUG_LIB_DIR "${GOOGLE_BENCHMARK_ROOT}/Debug/lib")
set(GOOGLE_BENCHMARK_RELEASE_LIB_DIR "${GOOGLE_BENCHMARK_ROOT}/Release/lib")

if(NOT EXISTS "${GOOGLE_BENCHMARK_INCLUDE_DIR}/benchmark/benchmark.h")
    message(FATAL_ERROR "Google Benchmark is not installed. Run ${CMAKE_CURRENT_LIST_DIR}/Install.bat first.")
endif()

function(slug_import_google_benchmark_target target debug_library release_library)
    if(NOT TARGET ${target})
        add_library(${target} STATIC IMPORTED GLOBAL)
        set_target_properties(${target} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GOOGLE_BENCHMARK_INCLUDE_DIR}"
            INTERFACE_COMPILE_DEFINITIONS BENCHMARK_STATIC_DEFINE
            IMPORTED_CONFIGURATIONS "Debug;Release"
            IMPORTED_LOCATION_DEBUG "${debug_library}"
            IMPORTED_LOCATION_RELEASE "${release_library}"
            MAP_IMPORTED_CONFIG_OPTIMIZE Release
            MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
            MAP_IMPORTED_CONFIG_MINSIZEREL Release
        )
    endif()
endfunction()

slug_import_google_benchmark_target(benchmark
    "${GOOGLE_BENCHMARK_DEBUG_LIB_DIR}/benchmark.lib"
    "${GOOGLE_BENCHMARK_RELEASE_LIB_DIR}/benchmark.lib"
)
slug_import_google_benchmark_target(benchmark_main
    "${GOOGLE_BENCHMARK_DEBUG_LIB_DIR}/benchmark_main.lib"
    "${GOOGLE_BENCHMARK_RELEASE_LIB_DIR}/benchmark_main.lib"
)

if(WIN32)
    set_property(TARGET benchmark APPEND PROPERTY INTERFACE_LINK_LIBRARIES shlwapi)
endif()
set_target_properties(benchmark_main PROPERTIES INTERFACE_LINK_LIBRARIES benchmark)

if(NOT TARGET benchmark::benchmark)
    add_library(benchmark::benchmark ALIAS benchmark)
endif()
if(NOT TARGET benchmark::benchmark_main)
    add_library(benchmark::benchmark_main ALIAS benchmark_main)
endif()
