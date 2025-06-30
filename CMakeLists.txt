cmake_minimum_required(VERSION 3.5)

add_compile_options(
    -Wno-deprecated-declarations
    -Wno-missing-field-initializers
    -Wno-maybe-uninitialized
    -Wno-unused-function
    -Wno-unused-variable
)

set(CMAKE_CXX_STANDARD    17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(Command_Voice)
