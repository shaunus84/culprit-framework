cmake_minimum_required(VERSION 3.14)
project(culprit-framework)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(dependencies)
add_subdirectory(unique-key-generator)
add_subdirectory(culprit-framework)
add_subdirectory(test)
