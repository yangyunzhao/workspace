cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(test_rbtree)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_RELEASE_POSTFIX "")

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(test_rbtree ${SRC})
add_executable(test_rbtree_clock ${SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../util)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
target_compile_definitions(test_rbtree_clock PUBLIC USE_CLOCK)