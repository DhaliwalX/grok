set(GROK_JIT_SOURCE_FILES "")
add_subdirectory(./codegen)

# find LLVMConfig.cmake
find_package(LLVM REQUIRED CONFIG)
# find_package(LLVM REQUIRED COMPONENTS core native mcjit)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(..)
add_definitions(${LLVM_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -DNO_EMIT_FUNCTION")
add_executable(jit ${GROK_JIT_SOURCE_FILES})
llvm_map_components_to_libnames(llvm_libs support core irreader mcjit native)
target_link_libraries(jit parser_jit ${llvm_libs} dl z m readline ncurses pthread)
