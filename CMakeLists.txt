project(openAL-Examples)
cmake_minimum_required(VERSION 2.6)

# the macro below enable the shadow build concept see http://brunoabinader.com/?p=29
set(CMAKE_MODULE_PATH ${openAL-Examples_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")

add_subdirectory(openal-example-1)

FILE(COPY ${CMAKE_SOURCE_DIR}/data
    DESTINATION ${CMAKE_BINARY_DIR})
