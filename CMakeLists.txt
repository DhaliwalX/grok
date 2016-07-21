cmake_minimum_required (VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project (grok)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif(NOT CMAKE_BUILD_TYPE)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++14 support.")
endif()

set(GROK_SOURCE_FILES "")
set(GROK_SHELL_SOURCE_FILES "")
add_subdirectory(./src/vm)
add_subdirectory(./src/grok)
add_subdirectory(./src/object)
add_subdirectory(./src/libs)
add_subdirectory(./src/lexer)
add_subdirectory(./src/common)
add_subdirectory(./src/input)
add_subdirectory(./src/parser)

option(GROK_JIT "Build experimental JIT support" OFF)

if (GROK_JIT)
    add_subdirectory(./src/jit)
endif(GROK_JIT)
# find readline library
find_package(Readline REQUIRED)
# find boost
find_package(Boost COMPONENTS system program_options iostreams
    filesystem REQUIRED)

find_package(Threads REQUIRED)

set (Boost_USE_MULTITHREAD ON)
# include directories
include_directories(./src)
add_library(grok ${GROK_SOURCE_FILES})
target_link_libraries(grok ${Boost_LIBRARIES} pthread ${READLINE_LIBRARIES})
add_executable(shell ${GROK_SHELL_SOURCE_FILES};${PROJECT_SOURCE_DIR}/main/main.cc)
target_link_libraries(shell ${LIBS} parser grok ${READLINE_LIBRARIES} ${Boost_LIBRARIES} pthread)

set(GROK_TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/test.cc)
add_executable(runtest ${GROK_TEST_SRC_FILES})
target_link_libraries(runtest ${LIBS} parser grok ${READLINE_LIBRARIES} ${Boost_LIBRARIES} pthread)

