cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(periodic-function
    VERSION 1.0.0
    LANGUAGES CXX
)

# if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
#    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
# endif()

# options
include(cmake/PeriodicFunctionOptions.cmake)

# dependencies

# CPM.cmake
include(cmake/CPM.cmake)

# System threading library
find_package(Threads REQUIRED)

# Make the package installable
CPMAddPackage(
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.0
)

# compiler warnings
include(cmake/CompilerWarnings.cmake)

add_library(project-warnings INTERFACE)
set_project_warnings(project-warnings)

if(NOT TEST_INSTALLED_VERSION)
  set(project_headers
    include/periodic_function/periodic_function.hpp
  )

  add_library(${PROJECT_NAME} INTERFACE)
  add_library(dp::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
  target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)
  
  # being a cross-platform target, we enforce standards conformance on MSVC
  target_compile_options(${PROJECT_NAME} INTERFACE "$<$<BOOL:${MSVC}>:/permissive->")

  target_include_directories(${PROJECT_NAME}
    INTERFACE
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
  )

  if(MSVC_IDE)
    add_custom_target(${PROJECT_NAME}.headers)
    set_target_properties(${PROJECT_NAME}.headers PROPERTIES SOURCES ${project_headers})
  endif()

  # ---- Create an installable target ----
  # this allows users to install and find the library via `find_package()`.
  packageProject(
    NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    BINARY_DIR ${PROJECT_BINARY_DIR}
    INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
    INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  )
endif()

if(BUILD_TESTS)
  # have to do this at the top level directory before adding sub-directories
  enable_testing()
  add_subdirectory(tests)
endif()
