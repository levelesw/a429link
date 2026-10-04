# Copyright 2026 levelesw
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.20)

project(a429link_consumer LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    a429link
)

add_executable(a429link_consumer main.cc parity.cc)
target_link_libraries(
    a429link_consumer
    PRIVATE a429link::a429link
)