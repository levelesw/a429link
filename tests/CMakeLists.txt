# Copyright 2026 levelesw
# SPDX-License-Identifier: BSD-2-Clause

find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
        GIT_SHALLOW TRUE
    )

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(a429link_word_test word_test.cc)
target_link_libraries(
    a429link_word_test
    PRIVATE
        a429link::a429link
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(a429link_word_test TEST_PREFIX "a429link.word.")
