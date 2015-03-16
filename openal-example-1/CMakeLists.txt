set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(openAL-Examples_SOURCES main.c)

add_executable(example-1 main.c)

find_package(OpenAL REQUIRED)
if(OpenAL_FOUND)
    include_directories(${OpenAL_INCLUDE_DIR})
    target_link_libraries(example-1 ${OpenAL_LIBRARIES})
endif(OpenAL_FOUND)

find_package(ALUT REQUIRED)
if(ALUT_FOUND)
    include_directories(${ALUT_INCLUDE_DIR})
    target_link_libraries(example-1 ${ALUT_LIBRARIES})
endif(ALUT_FOUND)

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sample1.wav
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

FILE(COPY ${CMAKE_SOURCE_DIR}/data/wav/about_time.wav
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/openal-example-1/sample1.wav
    DESTINATION openal-example-1)
