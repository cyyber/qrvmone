# qrvmone: Fast Quantum Resistant Virtual Machine implementation
# Copyright 2018-2020 The evmone Authors.
# SPDX-License-Identifier: Apache-2.0

# The internal qrvmone unit tests. The generic QRVM ones are also built in.
add_executable(qrvmone-unittests)
target_sources(
    qrvmone-unittests PRIVATE
    analysis_test.cpp
    bytecode_test.cpp
    qrvm_fixture.cpp
    qrvm_fixture.hpp
    qrvm_test.cpp
    qrvm_calls_test.cpp
    qrvm_control_flow_test.cpp
    qrvm_eip2929_test.cpp
    qrvm_eip3198_basefee_test.cpp
    qrvm_eip3855_push0_test.cpp
    qrvm_eip3860_initcode_test.cpp
    qrvm_memory_test.cpp
    qrvm_state_test.cpp
    qrvm_storage_test.cpp
    qrvm_other_test.cpp
    qrvm_benchmark_test.cpp
    qrvmone_test.cpp
    execution_state_test.cpp
    instructions_test.cpp
    state_bloom_filter_test.cpp
    state_mpt_hash_test.cpp
    state_mpt_test.cpp
    state_new_account_address_test.cpp
    state_rlp_test.cpp
    state_transition.hpp
    state_transition.cpp
    state_transition_block_test.cpp
    state_transition_create_test.cpp
    statetest_loader_block_info_test.cpp
    statetest_loader_test.cpp
    statetest_loader_tx_test.cpp
    statetest_logs_hash_test.cpp
    tracing_test.cpp
)
target_link_libraries(qrvmone-unittests PRIVATE qrvmone qrvmone::state qrvmone::statetestutils testutils qrvmc::instructions GTest::gtest GTest::gtest_main)
target_include_directories(qrvmone-unittests PRIVATE ${qrvmone_private_include_dir})

gtest_discover_tests(qrvmone-unittests TEST_PREFIX ${PROJECT_NAME}/unittests/)

option(QRVMONE_QRVM_TEST_TOOL "Enable QRVM unit testing tool for QRVMC implementations (not maintained)" OFF)
if(QRVMONE_QRVM_TEST_TOOL)
    # The qrvm-test tool that contains the all qrvm-unittests and loads VMs as QRVMC modules.
    add_executable(qrvm-test main.cpp)
    target_link_libraries(qrvm-test PRIVATE qrvm-unittests testutils qrvmc::qrvmc qrvmc::loader GTest::gtest)
endif()

# Provide the project version to selected source files.
set_source_files_properties(
    qrvmone_test.cpp
    main.cpp
    PROPERTIES COMPILE_DEFINITIONS PROJECT_VERSION="${PROJECT_VERSION}"
)
