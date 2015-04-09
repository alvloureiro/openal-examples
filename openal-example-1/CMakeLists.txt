set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(openAL-Examples_SOURCES main.c)

add_executable(example-1 main.c)

if(OpenAL_FOUND)
    target_link_libraries(example-1 ${OpenAL_LIBRARIES})
endif(OpenAL_FOUND)
if(ALUT_FOUND)
    target_link_libraries(example-1 ${ALUT_LIBRARIES})
endif(ALUT_FOUND)

