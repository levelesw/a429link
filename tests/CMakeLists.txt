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

add_executable(a429link_field_test field_test.cc)
target_link_libraries(
    a429link_field_test
    PRIVATE
        a429link::a429link
        GTest::gtest_main
)

add_executable(
    a429link_a429_test
    a429/parity_test.cc
    a429/standard_layout_test.cc
)
target_link_libraries(
    a429link_a429_test
    PRIVATE
        a429link::a429link
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(a429link_word_test TEST_PREFIX "a429link.word.")
gtest_discover_tests(a429link_field_test TEST_PREFIX "a429link.field.")
gtest_discover_tests(a429link_a429_test TEST_PREFIX "a429link.a429.")

option(
    A429LINK_WARNINGS_AS_ERRORS
    "Treat warnings in a429link tests as errors"
    OFF
)

if(A429LINK_WARNINGS_AS_ERRORS
   AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    foreach(test_target IN ITEMS
            a429link_word_test
            a429link_field_test
            a429link_a429_test)
        target_compile_options(
            ${test_target}
            PRIVATE -Wall -Wextra -Wpedantic -Werror
        )
    endforeach()
endif()
