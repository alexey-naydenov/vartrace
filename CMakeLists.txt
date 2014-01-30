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
enable_testing()

include(ExternalProject)

ExternalProject_Add(
  gtest
  SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
  INSTALL_COMMAND "")
ExternalProject_Get_Property(gtest source_dir)
include_directories(${source_dir}/include)
ExternalProject_Get_Property(gtest binary_dir)
set(GTEST_LIBS ${binary_dir}/libgtest.a)

add_subdirectory("trunk")
add_subdirectory("doc")


