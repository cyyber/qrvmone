# qrvmone: Fast Quantum Resistant Virtual Machine implementation
# Copyright 2021 The zvmone Authors.
# SPDX-License-Identifier: Apache-2.0

set(PREFIX ${PROJECT_NAME}/integration)

get_target_property(QRVMONE_LIB_TYPE qrvmone TYPE)
if(QRVMONE_LIB_TYPE STREQUAL SHARED_LIBRARY)

    add_test(NAME ${PREFIX}/histogram COMMAND $<TARGET_FILE:qrvmc::tool> --vm $<TARGET_FILE:qrvmone>,histogram run 6000a0a0a0a00101010200)
    set_tests_properties(
        ${PREFIX}/histogram PROPERTIES PASS_REGULAR_EXPRESSION
        "--- # HISTOGRAM depth=0
opcode,count
STOP,1
ADD,3
MUL,1
PUSH1,1
DUP1,4
")

    add_test(NAME ${PREFIX}/trace COMMAND $<TARGET_FILE:qrvmc::tool> --vm $<TARGET_FILE:qrvmone>,trace run 60006002a00103)
    set_tests_properties(
        ${PREFIX}/trace PROPERTIES PASS_REGULAR_EXPRESSION
        "{\"depth\":0,\"rev\":\"Zond\",\"static\":false}
{\"pc\":0,\"op\":96,\"opName\":\"PUSH1\",\"gas\":0xf4240,\"stack\":\\[\\],\"memorySize\":0}
{\"pc\":2,\"op\":96,\"opName\":\"PUSH1\",\"gas\":0xf423d,\"stack\":\\[\"0x0\"\\],\"memorySize\":0}
{\"pc\":4,\"op\":160,\"opName\":\"DUP1\",\"gas\":0xf423a,\"stack\":\\[\"0x0\",\"0x2\"\\],\"memorySize\":0}
{\"pc\":5,\"op\":1,\"opName\":\"ADD\",\"gas\":0xf4237,\"stack\":\\[\"0x0\",\"0x2\",\"0x2\"\\],\"memorySize\":0}
{\"pc\":6,\"op\":3,\"opName\":\"SUB\",\"gas\":0xf4234,\"stack\":\\[\"0x0\",\"0x4\"\\],\"memorySize\":0}
{\"error\":null,\"gas\":0xf4231,\"gasUsed\":0xf,\"output\":\"\"}
")

endif()

add_subdirectory(statetest)

get_property(ALL_TESTS DIRECTORY PROPERTY TESTS)
set_tests_properties(${ALL_TESTS} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/integration-%p.profraw)
