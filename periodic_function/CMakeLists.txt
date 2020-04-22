cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(periodic-fucntion
    VERSION 1.0.0
    LANGUAGES CXX
)

set(project_headers
  include/periodic_function/periodic_function.hpp
)

add_library(${PROJECT_NAME} INTERFACE)
add_library(dp::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_include_directories(${PROJECT_NAME}
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

if(MSVC_IDE)
  add_custom_target(${PROJECT_NAME}.headers)
  set_target_properties(${PROJECT_NAME}.headers PROPERTIES SOURCES ${project_headers})
endif()

if(BUILD_TESTS)
  add_subdirectory(tests)
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