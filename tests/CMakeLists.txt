cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(periodic-function-tests
  LANGUAGES CXX
)

# dependencies

# add clang format targets
CPMAddPackage(
  NAME Format.cmake
  GITHUB_REPOSITORY TheLartians/Format.cmake
  VERSION 1.1
)


CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.3.7
)

if (TEST_INSTALLED_VERSION)
  find_package(periodic-function REQUIRED)
endif()

# create binary
set(testing_sources
  src/main.cpp
  src/periodic_function_tests.cpp
)

add_executable(${PROJECT_NAME} ${testing_sources})
target_link_libraries(${PROJECT_NAME} doctest periodic-function project-warnings)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 COMPILE_FLAGS "-Wall -pedantic -Wextra -Werror")
else()
  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
endif()

# use doctest_discover_tests for better CTest support
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(${PROJECT_NAME})
