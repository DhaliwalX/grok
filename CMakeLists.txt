cmake_minimum_required (VERSION 2.6)
project (grok)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")
file (GLOB INCLUDE_DIRS relative ${CMAKE_SOURCE_DIR}/src/*)
set (GROK_SOURCE_FILES main/main.cc)
add_subdirectory(src/lexer)
add_subdirectory(src/parser)

# include directories
include_directories(./src/parser)
include_directories(./src/lexer)
include_directories(./src/common)
include_directories(./src/object)
add_executable(grok ${GROK_SOURCE_FILES})
target_link_libraries(grok src/parser src/lexer)

