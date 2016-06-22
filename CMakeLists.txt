cmake_minimum_required (VERSION 2.6)
project (grok)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
file (GLOB INCLUDE_DIRS relative ${CMAKE_SOURCE_DIR}/src/*)
set (GROK_SOURCE_FILES main/main.cc)
add_subdirectory(src/parser)
add_subdirectory(src/lexer)

add_executable(grok ${GROK_SOURCE_FILES})
target_link_libraries(grok src/parser src/lexer)

