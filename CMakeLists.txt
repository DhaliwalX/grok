cmake_minimum_required (VERSION 2.6)
project (grok)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall")
file (GLOB INCLUDE_DIRS relative ${CMAKE_SOURCE_DIR}/src/*)
set (SOURCE_FILES main/main.cc)


# include directories
include_directories(./src)

foreach (directory ${INCLUDE_DIRS})
    file (GLOB files relative ${directory}/*)
    foreach (fil ${files})
        set (SOURCE_FILES "${SOURCE_FILES};${fil}")
    endforeach(fil ${files})
endforeach(directory ${INCLUDE_DIRS})

add_executable(grok ${SOURCE_FILES})

