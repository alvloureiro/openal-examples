set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(pitch-shift_SOURCES main.c)

add_executable(pitch-shift main.c)

if(OpenAL_FOUND)
    target_link_libraries(pitch-shift ${OpenAL_LIBRARIES})
endif(OpenAL_FOUND)
if(ALUT_FOUND)
    target_link_libraries(pitch-shift ${ALUT_LIBRARIES})
endif(ALUT_FOUND)

