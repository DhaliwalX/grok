cmake_minimum_required (VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project (grok)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall")
file (GLOB INCLUDE_DIRS relative ${CMAKE_SOURCE_DIR}/src/*)
set (SOURCE_FILES main/main.cc)

# find readline library
find_package(Readline REQUIRED)

# find boost
find_package(Boost COMPONENTS program_options iostreams REQUIRED)

find_package (Threads REQUIRED)

set (Boost_USE_MULTITHREAD ON)

# include directories
include_directories(./src)

foreach (directory ${INCLUDE_DIRS})
    file (GLOB files relative ${directory}/*)
    foreach (fil ${files})
        set (SOURCE_FILES "${SOURCE_FILES};${fil}")
    endforeach(fil ${files})
endforeach(directory ${INCLUDE_DIRS})

add_executable(grok ${SOURCE_FILES})
target_link_libraries(grok ${READLINE_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)
