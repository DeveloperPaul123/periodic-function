cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(periodic-function)

if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

include(cmake/PeriodicFunctionOptions.cmake)
include(cmake/PeriodicFunctionDependencies.cmake)
include(cmake/CompilerWarnings.cmake)

if(BUILD_TESTS) 
    # have to do this at the top level directory before adding sub-directories
    enable_testing()
endif()

add_library(project-warnings INTERFACE)
set_project_warnings(project-warnings)

add_subdirectory(periodic_function)
