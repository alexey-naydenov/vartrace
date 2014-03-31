cmake_minimum_required(VERSION 2.8)

project(vartrace)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -g")

set(vartrace_VERSION_MAJOR 0)
set(vartrace_VERSION_MINOR 1)

configure_file(
  "${PROJECT_SOURCE_DIR}/trunk/${PROJECT_NAME}_config.h.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")

# boost
find_package(Boost COMPONENTS program_options filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# testing include
enable_testing()

find_library(GTEST_LIB gtest)

include_directories()
add_subdirectory("trunk")
add_subdirectory("doc")


