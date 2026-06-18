// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2022 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

/// This file contains QRVM unit tests for EIP-3855 "PUSH0 instruction"
/// https://eips.ethereum.org/EIPS/eip-3855

#include "qrvm_fixture.hpp"

using namespace qrvmc::literals;
using qrvmone::test::qrvm;

TEST_P(qrvm, push0)
{
    rev = QRVMC_ZOND;
    execute(OP_PUSH0 + ret_top());
    EXPECT_GAS_USED(QRVMC_SUCCESS, 17);
    EXPECT_OUTPUT_INT(0);
}

TEST_P(qrvm, push0_return_empty)
{
    rev = QRVMC_ZOND;
    execute(bytecode{} + OP_PUSH0 + OP_PUSH0 + OP_RETURN);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 4);
    EXPECT_EQ(result.output_size, 0);
}

TEST_P(qrvm, push0_full_stack)
{
    rev = QRVMC_ZOND;
    execute(1024 * bytecode{OP_PUSH0});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 1024 * 2);
}

TEST_P(qrvm, push0_stack_overflow)
{
    rev = QRVMC_ZOND;
    execute(1025 * bytecode{OP_PUSH0});
    EXPECT_STATUS(QRVMC_STACK_OVERFLOW);
}
