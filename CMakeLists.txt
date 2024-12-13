# CMakeLists.txt for building an executable that links to a custom library
cmake_minimum_required(VERSION 3.16)

project(1Brc VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-extra-arg=-std=c++23")


add_subdirectory(src/lib)
include_directories(src/lib)

foreach(i RANGE 1 13)
  set(target_name "aoc${i}")
  add_executable(${target_name} src/aoc${i}.cpp)
  target_link_libraries(${target_name} PRIVATE Utils)
  target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  add_custom_target(run_${target_name} COMMAND $<TARGET_FILE:${target_name}> DEPENDS ${target_name})
endforeach()

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "run_aoc*")


set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -fvectorize")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
