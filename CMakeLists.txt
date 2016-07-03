cmake_minimum_required (VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project (grok)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall")

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cc)
set(SRC_FILES ${SRC_FILES};${PROJECT_SOURCE_DIR}/main/main.cc)
# find readline library
find_package(Readline REQUIRED)
# find boost
find_package(Boost COMPONENTS program_options iostreams REQUIRED)
find_package (Threads)

set (Boost_USE_MULTITHREAD ON)
# include directories
include_directories(./src)

add_executable(grok ${SRC_FILES})
target_link_libraries(grok ${READLINE_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)
