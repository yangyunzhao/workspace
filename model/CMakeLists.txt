cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(simu_model)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_RELEASE_POSTFIX "")

file(GLOB SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(model ${SRC})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third/json)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_subdirectory(test_rbt)