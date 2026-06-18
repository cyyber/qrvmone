// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2021 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

/// This file contains QRVM unit tests for EIP-3198 "BASEFEE opcode"
/// https://eips.ethereum.org/EIPS/eip-3198

#include "qrvm_fixture.hpp"

using namespace qrvmc::literals;
using qrvmone::test::qrvm;

TEST_P(qrvm, basefee_nominal_case)
{
    // https://eips.ethereum.org/EIPS/eip-3198#nominal-case
    rev = QRVMC_ZOND;
    host.tx_context.block_base_fee = qrvmc::bytes64{7};

    execute(bytecode{} + OP_BASEFEE + OP_STOP);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2);

    execute(bytecode{} + OP_BASEFEE + ret_top());
    EXPECT_GAS_USED(QRVMC_SUCCESS, 17);
    EXPECT_OUTPUT_INT(7);
}
