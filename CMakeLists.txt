cmake_minimum_required(VERSION 3.14)
project(culprit-framework VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(culprit-framework)

option(CULPRIT_BUILD_TESTS "Build tests for culprit-framework" OFF)

if(CULPRIT_BUILD_TESTS)
    add_subdirectory(dependencies)
    add_subdirectory(test)
endif()
