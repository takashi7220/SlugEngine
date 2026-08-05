set(TINYGLTF_ROOT "${CMAKE_CURRENT_LIST_DIR}/.tmp/install")
set(TINYGLTF_INCLUDE_DIR "${TINYGLTF_ROOT}/Release/include")

if(NOT EXISTS "${TINYGLTF_INCLUDE_DIR}/tiny_gltf.h")
    message(FATAL_ERROR "tinygltf is not installed. Run ${CMAKE_CURRENT_LIST_DIR}/Install.bat first.")
endif()

if(NOT TARGET tinygltf)
    add_library(tinygltf INTERFACE IMPORTED GLOBAL)
    set_target_properties(tinygltf PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${TINYGLTF_INCLUDE_DIR}"
    )
endif()

if(NOT TARGET tinygltf::tinygltf)
    add_library(tinygltf::tinygltf ALIAS tinygltf)
endif()
