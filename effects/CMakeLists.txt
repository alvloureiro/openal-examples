set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(effects_SOURCES main.c)

add_executable(effects main.c)

if(OpenAL_FOUND)
    target_link_libraries(effects ${OpenAL_LIBRARIES})
endif(OpenAL_FOUND)
if(ALUT_FOUND)
    target_link_libraries(effects ${ALUT_LIBRARIES})
endif(ALUT_FOUND)

