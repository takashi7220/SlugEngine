set(GRPC_ROOT "${CMAKE_CURRENT_LIST_DIR}/.tmp/install")
set(GRPC_DEBUG_ROOT "${GRPC_ROOT}/Debug")
set(GRPC_RELEASE_ROOT "${GRPC_ROOT}/Release")
set(GRPC_DEBUG_CMAKE_DIR "${GRPC_DEBUG_ROOT}/lib/cmake/grpc")
set(GRPC_RELEASE_CMAKE_DIR "${GRPC_RELEASE_ROOT}/lib/cmake/grpc")

if(NOT EXISTS "${GRPC_RELEASE_CMAKE_DIR}/gRPCConfig.cmake")
    message(FATAL_ERROR "gRPC is not installed. Run ${CMAKE_CURRENT_LIST_DIR}/Install.bat first.")
endif()

list(PREPEND CMAKE_PREFIX_PATH "${GRPC_RELEASE_ROOT}" "${GRPC_DEBUG_ROOT}")
include("${GRPC_RELEASE_CMAKE_DIR}/gRPCConfig.cmake")

foreach(slug_grpc_debug_import IN ITEMS gRPCTargets-debug.cmake gRPCPluginTargets-debug.cmake)
    if(EXISTS "${GRPC_DEBUG_CMAKE_DIR}/${slug_grpc_debug_import}")
        set(_IMPORT_PREFIX "${GRPC_DEBUG_ROOT}")
        include("${GRPC_DEBUG_CMAKE_DIR}/${slug_grpc_debug_import}")
        unset(_IMPORT_PREFIX)
    endif()
endforeach()

get_property(slug_grpc_imported_targets DIRECTORY PROPERTY IMPORTED_TARGETS)
foreach(slug_grpc_target IN LISTS slug_grpc_imported_targets)
    set_target_properties(${slug_grpc_target} PROPERTIES
        MAP_IMPORTED_CONFIG_OPTIMIZE Release
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
        MAP_IMPORTED_CONFIG_MINSIZEREL Release
    )
endforeach()

if(TARGET gRPC::grpc AND NOT TARGET grpc)
    add_library(grpc ALIAS gRPC::grpc)
endif()
if(TARGET gRPC::grpc++ AND NOT TARGET grpc++)
    add_library(grpc++ ALIAS gRPC::grpc++)
endif()
if(TARGET gRPC::gpr AND NOT TARGET gpr)
    add_library(gpr ALIAS gRPC::gpr)
endif()
