// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2021 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

/// This file contains QRVM unit tests for EIP-2929 "Gas cost increases for state access opcodes"
/// https://eips.ethereum.org/EIPS/eip-2929

#include "qrvm_fixture.hpp"

using namespace qrvmc::literals;
using qrvmone::test::qrvm;

TEST_P(qrvm, eip2929_case1)
{
    // https://gist.github.com/holiman/174548cad102096858583c6fbbb0649a#case-1
    rev = QRVMC_ZOND;
    msg.sender = "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_address;
    msg.recipient = "Q00000000000000000000000000000000000000000000000000000000000000000000000000000000636F6E7472616374"_address;
    const bytecode code =
        "0x60013f5060023b506003315060f13f5060f23b5060f3315060f23f5060f33b5060f1315032315030315000";

    execute(code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 8653);
    EXPECT_EQ(result.output_size, 0);

    const auto& r = host.recorded_account_accesses;
    ASSERT_EQ(r.size(), 24);
    EXPECT_EQ(r[0], msg.sender);
    EXPECT_EQ(r[1], msg.recipient);
    EXPECT_EQ(r[2], "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001"_address);
    EXPECT_EQ(r[3], "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001"_address);
    EXPECT_EQ(r[4], "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002"_address);
    EXPECT_EQ(r[5], "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002"_address);
    EXPECT_EQ(r[6], "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003"_address);
    EXPECT_EQ(r[7], "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003"_address);
    EXPECT_EQ(r[8], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f1"_address);
    EXPECT_EQ(r[9], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f1"_address);
    EXPECT_EQ(r[10], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f2"_address);
    EXPECT_EQ(r[11], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f2"_address);
    EXPECT_EQ(r[12], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f3"_address);
    EXPECT_EQ(r[13], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f3"_address);
    EXPECT_EQ(r[14], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f2"_address);
    EXPECT_EQ(r[15], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f2"_address);
    EXPECT_EQ(r[16], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f3"_address);
    EXPECT_EQ(r[17], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f3"_address);
    EXPECT_EQ(r[18], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f1"_address);
    EXPECT_EQ(r[19], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000f1"_address);
    EXPECT_EQ(r[20], "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_address);
    EXPECT_EQ(r[21], "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_address);
    EXPECT_EQ(r[22], msg.recipient);
    EXPECT_EQ(r[23], msg.recipient);
}

TEST_P(qrvm, eip2929_case2)
{
    // https://gist.github.com/holiman/174548cad102096858583c6fbbb0649a#case-2
    rev = QRVMC_ZOND;
    msg.sender = "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_address;
    msg.recipient = "Q00000000000000000000000000000000000000000000000000000000000000000000000000000000636F6E7472616374"_address;
    const bytecode code = "0x60006000600060ff3c60006000600060ff3c600060006000303c00";

    execute(code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2835);
    EXPECT_EQ(result.output_size, 0);

    const auto& r = host.recorded_account_accesses;
    ASSERT_EQ(r.size(), 5);
    EXPECT_EQ(r[0], msg.sender);
    EXPECT_EQ(r[1], msg.recipient);
    EXPECT_EQ(r[2], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ff"_address);
    EXPECT_EQ(r[3], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ff"_address);
    EXPECT_EQ(r[4], msg.recipient);
}

TEST_P(qrvm, eip2929_case3)
{
    // https://gist.github.com/holiman/174548cad102096858583c6fbbb0649a#case-3
    rev = QRVMC_ZOND;
    msg.sender = "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_address;
    msg.recipient = "Q00000000000000000000000000000000000000000000000000000000000000000000000000000000636F6E7472616374"_address;
    const bytecode code = "0x60015450601160015560116002556011600255600254600154";

    execute(code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 44529);
    EXPECT_EQ(result.output_size, 0);
}

TEST_P(qrvm, eip2929_case4)
{
    // https://gist.github.com/holiman/174548cad102096858583c6fbbb0649a#case-4
    rev = QRVMC_ZOND;
    msg.sender = "Q000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_address;
    msg.recipient = "Q00000000000000000000000000000000000000000000000000000000000000000000000000000000636F6E7472616374"_address;
    const bytecode code =
        "0x60008080808060046000f15060008080808060ff6000f15060008080808060ff6000fa50";

    execute(code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 6);
    EXPECT_EQ(result.output_size, 0);
}

TEST_P(qrvm, eip2929_balance_oog)
{
    rev = QRVMC_ZOND;
    const auto code = push(0x0a) + OP_BALANCE;

    execute(2603, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2603);

    host.recorded_account_accesses.clear();
    execute(2602, code);
    EXPECT_GAS_USED(QRVMC_OUT_OF_GAS, 2602);
}

TEST_P(qrvm, eip2929_extcodesize_oog)
{
    rev = QRVMC_ZOND;
    const auto code = push(0x0a) + OP_EXTCODESIZE;

    execute(2603, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2603);

    host.recorded_account_accesses.clear();
    execute(2602, code);
    EXPECT_GAS_USED(QRVMC_OUT_OF_GAS, 2602);
}

TEST_P(qrvm, eip2929_extcodecopy_oog)
{
    rev = QRVMC_ZOND;
    const auto code = push(0) + OP_DUP1 + OP_DUP1 + push(0x0a) + OP_EXTCODECOPY;

    execute(2612, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2612);

    host.recorded_account_accesses.clear();
    execute(2611, code);
    EXPECT_GAS_USED(QRVMC_OUT_OF_GAS, 2611);
}

TEST_P(qrvm, eip2929_extcodehash_oog)
{
    rev = QRVMC_ZOND;
    const auto code = push(0x0a) + OP_EXTCODEHASH;

    execute(2603, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2603);

    host.recorded_account_accesses.clear();
    execute(2602, code);
    EXPECT_GAS_USED(QRVMC_OUT_OF_GAS, 2602);
}

TEST_P(qrvm, eip2929_sload_cold)
{
    rev = QRVMC_ZOND;
    const auto code = push(1) + OP_SLOAD;

    const qrvmc::bytes64 key{1};
    host.accounts[msg.recipient].storage[key] = qrvmc::bytes64{2};
    ASSERT_EQ(host.accounts[msg.recipient].storage[key].access_status, QRVMC_ACCESS_COLD);
    execute(2103, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2103);
    EXPECT_EQ(host.accounts[msg.recipient].storage[key].access_status, QRVMC_ACCESS_WARM);

    host.accounts[msg.recipient].storage[key].access_status = QRVMC_ACCESS_COLD;
    execute(2102, code);
    EXPECT_GAS_USED(QRVMC_OUT_OF_GAS, 2102);
}

TEST_P(qrvm, eip2929_sload_two_slots)
{
    rev = QRVMC_ZOND;
    const qrvmc::bytes64 key0{0};
    const qrvmc::bytes64 key1{1};
    const auto code = push(key0) + OP_SLOAD + OP_POP + push(key1) + OP_SLOAD + OP_POP;

    execute(30000, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 4210);
    EXPECT_EQ(host.accounts[msg.recipient].storage[key0].access_status, QRVMC_ACCESS_WARM);
    EXPECT_EQ(host.accounts[msg.recipient].storage[key1].access_status, QRVMC_ACCESS_WARM);
}

TEST_P(qrvm, eip2929_sload_warm)
{
    rev = QRVMC_ZOND;
    const auto code = push(1) + OP_SLOAD;

    const qrvmc::bytes64 key{1};
    host.accounts[msg.recipient].storage[key] = {qrvmc::bytes64{2}, QRVMC_ACCESS_WARM};
    ASSERT_EQ(host.accounts[msg.recipient].storage[key].access_status, QRVMC_ACCESS_WARM);
    execute(103, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 103);
    EXPECT_EQ(host.accounts[msg.recipient].storage[key].access_status, QRVMC_ACCESS_WARM);

    execute(102, code);
    EXPECT_GAS_USED(QRVMC_OUT_OF_GAS, 102);
}

TEST_P(qrvm, eip2929_sstore_modify_cold)
{
    rev = QRVMC_ZOND;
    const auto code = sstore(1, 3);

    const qrvmc::bytes64 key{1};
    host.accounts[msg.recipient].storage[key] = qrvmc::bytes64{2};
    execute(5006, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 5006);
    EXPECT_EQ(host.accounts[msg.recipient].storage[key].current, qrvmc::bytes64{3});
    EXPECT_EQ(host.accounts[msg.recipient].storage[key].access_status, QRVMC_ACCESS_WARM);

    host.accounts[msg.recipient].storage[key] = qrvmc::bytes64{2};
    execute(5005, code);
    EXPECT_GAS_USED(QRVMC_OUT_OF_GAS, 5005);
    // The storage will be modified anyway, because the cost is checked after.
    EXPECT_EQ(host.accounts[msg.recipient].storage[key].current, qrvmc::bytes64{3});
    EXPECT_EQ(host.accounts[msg.recipient].storage[key].access_status, QRVMC_ACCESS_WARM);
}

TEST_P(qrvm, eip2929_delegatecall_cold)
{
    rev = QRVMC_ZOND;
    const auto code = delegatecall(0xde);
    auto& r = host.recorded_account_accesses;

    execute(2618, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2618);
    ASSERT_EQ(r.size(), 4);
    EXPECT_EQ(r[0], msg.sender);
    EXPECT_EQ(r[1], msg.recipient);
    EXPECT_EQ(r[2], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000de"_address);
    EXPECT_EQ(r[3], msg.sender);

    r.clear();
    execute(2617, code);
    EXPECT_GAS_USED(QRVMC_OUT_OF_GAS, 2617);
    ASSERT_EQ(r.size(), 3);
    EXPECT_EQ(r[0], msg.sender);
    EXPECT_EQ(r[1], msg.recipient);
    EXPECT_EQ(r[2], "Q0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000de"_address);
}
