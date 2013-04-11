cmake_minimum_required(VERSION 2.8)

project(vartrace)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(vartrace_VERSION_MAJOR 0)
set(vartrace_VERSION_MINOR 1)

configure_file(
  "${PROJECT_SOURCE_DIR}/trunk/${PROJECT_NAME}_config.h.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# testing include
set(GTEST_ROOT "${PROJECT_SOURCE_DIR}/gtest_build")
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory("trunk")
add_subdirectory("doc")


