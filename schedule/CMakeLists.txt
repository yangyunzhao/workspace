cmake_minimum_required(VERSION 3.10)
project(schedule)

# Set C++11 standard
set(CMAKE_CXX_STANDARD 17)

# Add the executable
add_executable(schedule source/main.cpp source/schedule.cpp source/algorithm_fetch.cpp source/algorithm_steal.cpp source/algorithm_mixed.cpp)

# Include cxxopts header
include_directories(${CMAKE_SOURCE_DIR}/include)