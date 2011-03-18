cmake_minimum_required (VERSION 2.6)

project (vartrace)

set(CMAKE_CXX_LINK_FLAGS "-Wl")

set (vartrace_VERSION_MAJOR 0)
set (vartrace_VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/trunk/${PROJECT_NAME}_config.h.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_config.h"
  )

include_directories ("${PROJECT_BINARY_DIR}")

enable_testing ()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory ("trunk")
add_subdirectory ("doc")


