cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(simu_model)

IF (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
ENDIF()

message(STATUS "cmake build-type is:${CMAKE_BUILD_TYPE}")

set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_RELEASE_POSTFIX "")

file(GLOB SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(model ${SRC})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third/json)
include_directories(${CMAKE_SOURCE_DIR}/util)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_subdirectory(test_rbt)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    IF (${CMAKE_BUILD_TYPE} MATCHES "Debug")
        target_compile_options(model PUBLIC -g)
    ELSE()
        add_compile_options(-O3)
    ENDIF()
ENDIF()