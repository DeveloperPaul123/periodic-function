option(BUILD_TESTS "Turn on to build unit tests." ON)

include(CMakeDependentOption)
cmake_dependent_option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF "BUILD_TESTS" OFF)
