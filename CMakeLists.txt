# Copyright 2026 levelesw
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.20)

project(a429link LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

add_library(a429link INTERFACE)
add_library(a429link::a429link ALIAS a429link)

target_compile_features(a429link INTERFACE cxx_std_23)
target_include_directories(
    a429link
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    include(CTest)

    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()
