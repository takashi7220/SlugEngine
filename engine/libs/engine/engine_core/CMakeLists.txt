set(SOURCE_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_PATH_LIST ${SOURCE_ROOT_PATH}/public ${SOURCE_ROOT_PATH}/private)
set(PROJECT_NAME engine_core)

project(${PROJECT_NAME})

set(COMPILE_TARGET_FILE_LIST "")
foreach(SOURCE_PATH ${SOURCE_PATH_LIST})
    file(GLOB_RECURSE SOURCE_CODE_LIST ${SOURCE_PATH}/*.cpp ${SOURCE_PATH}/*.hpp ${SOURCE_PATH}/*.h ${SOURCE_PATH}/*.c)
    LIST(APPEND COMPILE_TARGET_FILE_LIST ${SOURCE_CODE_LIST} )
endforeach()

add_library(${PROJECT_NAME} STATIC ${COMPILE_TARGET_FILE_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_ROOT_PATH}/public)
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_ROOT_PATH}/private)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER libs/engine/)

target_link_libraries(${PROJECT_NAME} PUBLIC core)

create_source_group(${SOURCE_ROOT_PATH})
setup_options(${PROJECT_NAME})
