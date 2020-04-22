cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(periodic-function-tests
  LANGUAGES CXX
)

if (TEST_INSTALLED_VERSION)
  find_package(periodic-function REQUIRED)
endif()

# ---- Create binary ----
set(testing_sources
  src/main.cpp
  src/periodic_function_tests.cpp
)

add_executable(${PROJECT_NAME} ${testing_sources})
target_link_libraries(${PROJECT_NAME} doctest periodic-fucntion)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 COMPILE_FLAGS "-Wall -pedantic -Wextra -Werror")
else()
  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
endif()

# ---- Add GreeterTests ----

enable_testing() 

# use doctest_discover_tests for better CTest support
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(${PROJECT_NAME})

if (ENABLE_TEST_COVERAGE)
  set_target_properties(periodic-function PROPERTIES CXX_STANDARD 17 COMPILE_FLAGS "-O0 -g -fprofile-arcs -ftest-coverage --coverage")
  target_link_options(periodic-function PUBLIC "--coverage")
endif()
