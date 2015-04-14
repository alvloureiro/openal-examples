set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(filters_SOURCES main.c)

add_executable(filters main.c)

if(OpenAL_FOUND)
    target_link_libraries(filters ${OpenAL_LIBRARIES})
endif(OpenAL_FOUND)
if(ALUT_FOUND)
    target_link_libraries(filters ${ALUT_LIBRARIES})
endif(ALUT_FOUND)

