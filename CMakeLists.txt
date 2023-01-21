cmake_minimum_required(VERSION 3.15)

project(VariableDumper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(VariableDumper "")

set(VARIABLE_DUMPER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)

if(MSVC)
  target_compile_options(VariableDumper PRIVATE /W4 /WX)
else()
  target_compile_options(VariableDumper PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory(src)
add_subdirectory(test)
