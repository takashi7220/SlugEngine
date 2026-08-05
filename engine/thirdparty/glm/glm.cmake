set(GLM_ROOT "${CMAKE_CURRENT_LIST_DIR}/.tmp/install")
set(GLM_INCLUDE_DIR "${GLM_ROOT}/Release/include")

if(NOT EXISTS "${GLM_INCLUDE_DIR}/glm/glm.hpp")
    message(FATAL_ERROR "GLM is not installed. Run ${CMAKE_CURRENT_LIST_DIR}/Install.bat first.")
endif()

if(NOT TARGET glm-header-only)
    add_library(glm-header-only INTERFACE IMPORTED GLOBAL)
    set_target_properties(glm-header-only PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GLM_INCLUDE_DIR}"
    )
endif()

if(NOT TARGET glm)
    add_library(glm INTERFACE IMPORTED GLOBAL)
    set_target_properties(glm PROPERTIES
        INTERFACE_LINK_LIBRARIES glm-header-only
    )
endif()

if(NOT TARGET glm::glm-header-only)
    add_library(glm::glm-header-only ALIAS glm-header-only)
endif()
if(NOT TARGET glm::glm)
    add_library(glm::glm ALIAS glm)
endif()
