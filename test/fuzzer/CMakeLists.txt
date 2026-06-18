# qrvmone-fuzzer: LibFuzzer based testing tool for QRVMC-compatible QRVM implementations.
# Copyright 2019 The evmone Authors.
# SPDX-License-Identifier: Apache-2.0
include(ExternalProject)

get_target_property(type qrvmone TYPE)
if(NOT type STREQUAL STATIC_LIBRARY)
    message(FATAL_ERROR "The qrvmone must be built as static library")
endif()

if(fuzzing_coverage)
    set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=fuzzer")
else()
    string(REPLACE fuzzer-no-link fuzzer CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
endif()

add_executable(qrvmone-fuzzer fuzzer.cpp)
target_link_libraries(qrvmone-fuzzer PRIVATE qrvmone qrvmone::testutils qrvmc::mocked_host)
