// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2019-2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <gtest/gtest.h>
#include <intx/intx.hpp>
#include <test/utils/bytecode.hpp>
#include <qrvmc/mocked_host.hpp>

#define EXPECT_STATUS(STATUS_CODE)                                           \
    EXPECT_EQ(result.status_code, STATUS_CODE);                              \
    if constexpr (STATUS_CODE != QRVMC_SUCCESS && STATUS_CODE != QRVMC_REVERT) \
    {                                                                        \
        EXPECT_EQ(result.gas_left, 0);                                       \
    }                                                                        \
    (void)0

#define EXPECT_GAS_USED(STATUS_CODE, GAS_USED)  \
    EXPECT_EQ(result.status_code, STATUS_CODE); \
    EXPECT_EQ(gas_used, GAS_USED)

// Compare the 32-byte `result.output_data` against a big-endian uint256
// encoding of X. With the 64-byte-word migration `qrvmc_bytes32` is 64
// bytes wide, which can't accept an intx uint256 store; stage the
// uint256 into a local 32-byte buffer instead.
#define EXPECT_OUTPUT_INT(X)                                                      \
    ASSERT_EQ(result.output_size, sizeof(intx::uint256));                         \
    do                                                                            \
    {                                                                             \
        uint8_t _expected_be[sizeof(intx::uint256)] = {};                         \
        intx::be::store(_expected_be, intx::uint256{X});                          \
        EXPECT_EQ(hex({result.output_data, result.output_size}),                  \
            hex({_expected_be, sizeof(_expected_be)}));                           \
    } while (false)


namespace qrvmone::test
{
/// The "qrvm" test fixture with generic unit tests for QRVMC-compatible VM implementations.
class qrvm : public testing::TestWithParam<qrvmc::VM*>
{
protected:
    /// Reports if execution is done by qrvmone/Advanced.
    static bool is_advanced() noexcept;

    /// The VM handle.
    qrvmc::VM& vm;

    /// The QRVM revision for unit test execution. Zond by default.
    /// TODO: Add alias qrvmc::revision.
    qrvmc_revision rev = QRVMC_ZOND;

    /// The message to be executed by a unit test (with execute() method).
    /// TODO: Add qrvmc::message with default constructor.
    qrvmc_message msg{};

    /// The result of execution (available after execute() is invoked).
    qrvmc::Result result;

    /// The result output. Updated by execute().
    bytes_view output;

    /// The total amount of gas used during execution.
    int64_t gas_used = 0;

    qrvmc::MockedHost host;

    qrvm() noexcept : vm{*GetParam()} {}


    /// Executes the supplied code.
    ///
    /// @param gas    The gas limit for execution.
    /// @param code   The QRVM bytecode.
    /// @param input  The QRVM "calldata" input.
    /// The execution result will be available in the `result` field.
    /// The `gas_used` field  will be updated accordingly.
    void execute(int64_t gas, const bytecode& code, bytes_view input = {}) noexcept
    {
        msg.input_data = input.data();
        msg.input_size = input.size();
        msg.gas = gas;

        // Add EIP-2929 tweak.
        host.access_account(msg.sender);
        host.access_account(msg.recipient);

        result = vm.execute(host, rev, msg, code.data(), code.size());
        output = {result.output_data, result.output_size};
        gas_used = msg.gas - result.gas_left;
    }

    /// Executes the supplied code.
    ///
    /// @param code   The QRVM bytecode.
    /// @param input  The QRVM "calldata" input.
    /// The execution result will be available in the `result` field.
    /// The `gas_used` field  will be updated accordingly.
    void execute(const bytecode& code, bytes_view input = {}) noexcept
    {
        execute(std::numeric_limits<int64_t>::max(), code, input);
    }
};
}  // namespace qrvmone::test
