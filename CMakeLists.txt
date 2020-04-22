cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(periodic-function)

if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

include(cmake/PeriodicFunctionOptions.cmake)
include(cmake/PeriodicFunctionDependencies.cmake)

add_subdirectory(periodic_function)
