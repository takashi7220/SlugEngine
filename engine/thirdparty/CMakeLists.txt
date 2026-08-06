# googletest
if(SLUG_BUILD_TEST)
    execute_process(COMMAND cmd /c "${CMAKE_CURRENT_LIST_DIR}/googletest/Install.bat" 
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/googletest/" 
    RESULT_VARIABLE result)
    include(googletest/googletest.cmake)
endif()

# google_benchmark
if(SLUG_BUILD_TEST)
    execute_process(COMMAND cmd /c "${CMAKE_CURRENT_LIST_DIR}/google_benchmark/Install.bat" 
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/google_benchmark/" 
    RESULT_VARIABLE result)
    include(google_benchmark/google_benchmark.cmake)
endif()

#glm
if(SLUG_BUILD_TEST)
    execute_process(COMMAND cmd /c "${CMAKE_CURRENT_LIST_DIR}/glm/Install.bat" 
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/glm/" 
    RESULT_VARIABLE result)
    include(glm/glm.cmake)
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
    execute_process(COMMAND cmd /c "${CMAKE_CURRENT_LIST_DIR}/mimalloc/Install.bat" 
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/mimalloc/" 
    RESULT_VARIABLE result)
    include(mimalloc/mimalloc.cmake)
endif()