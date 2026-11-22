// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// SPDX-License-Identifier: Apache-2.0

/// This file contains QRVM unit tests for EIP-7939 "Count leading zeros (CLZ) opcode"
/// https://eips.ethereum.org/EIPS/eip-7939
///
/// The word is 512 bits wide here, so the count is over 512 bits: it is 512 for zero and,
/// for a value below 2^256, 256 more than the count of the EIP.

#include "qrvm_fixture.hpp"
#include <string>

using qrvmone::test::qrvm;

namespace
{
using uint512 = intx::uint<512>;

// The pieces of go-qrl's conformance vectors (core/vm/conformance/vectors.go at
// cyyber/go-qrl 416ac52), so that both runtimes assert the same results and gas.

/// CLZ: 5 gas.
const std::string op_clz = "1e";

/// MSTORE of the top of the stack at 0 and RETURN of those 64 bytes: 15 gas.
const std::string store_and_return_64 = "60005260406000f3";

/// A 64-byte word with the given hex digits at its low end.
std::string word_hex(const std::string& low_digits)
{
    return std::string(128 - low_digits.size(), '0') + low_digits;
}

/// PUSH64 of the value: every byte of the word is in the code, none comes from an instruction.
bytecode push_word(const uint512& value)
{
    uint8_t data[sizeof(value)]{};
    intx::be::store(data, value);
    return push(bytes_view{data, std::size(data)});
}

/// The 64 bytes of the value, as MSTORE writes them.
bytes word_bytes(const uint512& value)
{
    uint8_t data[sizeof(value)]{};
    intx::be::store(data, value);
    return bytes{data, std::size(data)};
}
}  // namespace

TEST_P(qrvm, clz_zero)
{
    execute(100000, bytecode{"6000" + op_clz + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 23);
    EXPECT_EQ(hex(output), word_hex("0200"));
}

TEST_P(qrvm, clz_one)
{
    execute(100000, bytecode{"6001" + op_clz + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 23);
    EXPECT_EQ(hex(output), word_hex("01ff"));
}

TEST_P(qrvm, clz_top_bit_of_256_bit_word)
{
    // PUSH32 of 2^255: the EIP counts no leading zero here, the 512-bit word has 256.
    execute(100000, bytecode{"7f80" + std::string(62, '0') + op_clz + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 23);
    EXPECT_EQ(hex(output), word_hex("0100"));
}

TEST_P(qrvm, clz_first_value_over_256_bits)
{
    // PUSH33 of 2^256: the first value that does not fit in 256 bits.
    execute(100000, bytecode{"8001" + std::string(64, '0') + op_clz + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 23);
    EXPECT_EQ(hex(output), word_hex("ff"));
}

TEST_P(qrvm, clz_top_bit)
{
    // 2^511, from PUSH1 1, PUSH2 511, SHL.
    execute(100000, bytecode{"60016101ff1b" + op_clz + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 29);
    EXPECT_EQ(hex(output), word_hex("00"));
}

TEST_P(qrvm, clz_all_ones)
{
    // 2^512 - 1, from PUSH1 0, NOT.
    execute(100000, bytecode{"600019" + op_clz + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 26);
    EXPECT_EQ(hex(output), word_hex("00"));
}

TEST_P(qrvm, clz_every_bit_position)
{
    // For every bit position i, 2^i is the smallest value whose top bit is i
    // and 2^(i+1) - 1 is the largest: both have 511 - i leading zeros.
    const auto return_word = mstore(0) + ret(0, 64);
    for (unsigned i = 0; i < 512; ++i)
    {
        const auto expected = word_bytes(uint512{511 - i});
        const auto low = uint512{1} << i;
        const auto high = low | (low - uint512{1});

        execute(clz(push_word(low)) + return_word);
        EXPECT_STATUS(QRVMC_SUCCESS);
        EXPECT_EQ(output, expected) << "2^" << i;

        execute(clz(push_word(high)) + return_word);
        EXPECT_STATUS(QRVMC_SUCCESS);
        EXPECT_EQ(output, expected) << "2^" << (i + 1) << " - 1";
    }
}

TEST_P(qrvm, clz_ignores_bits_below_the_top_one)
{
    // The top set bit is bit 300 in all of them, in the limb with index 4 of the eight.
    const auto top = uint512{1} << 300;
    for (const auto& x :
        {top, top | uint512{1}, top | (uint512{1} << 299), top | (uint512{1} << 64)})
    {
        execute(clz(push_word(x)) + ret_top());
        EXPECT_STATUS(QRVMC_SUCCESS);
        EXPECT_OUTPUT_INT(211);
    }
}

TEST_P(qrvm, clz_replaces_only_the_top_item)
{
    // The item below is still there for the ADD: 7 + clz(1) = 7 + 511.
    execute(push(7) + clz(1) + OP_ADD + ret_top());
    EXPECT_STATUS(QRVMC_SUCCESS);
    EXPECT_OUTPUT_INT(518);

    // And the count is a word like any other: clz(clz(0)) = clz(512) = 502.
    execute(clz(clz(0)) + ret_top());
    EXPECT_STATUS(QRVMC_SUCCESS);
    EXPECT_OUTPUT_INT(502);
}

TEST_P(qrvm, clz_cost)
{
    // 3 for the push and 5 for CLZ.
    const auto code = clz(1);
    execute(8, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 8);

    execute(7, code);
    EXPECT_STATUS(QRVMC_OUT_OF_GAS);
}

TEST_P(qrvm, clz_static_mode)
{
    // CLZ reads and writes the stack only: it works in a static call.
    msg.flags = QRVMC_STATIC;
    execute(clz(1) + ret_top());
    EXPECT_STATUS(QRVMC_SUCCESS);
    EXPECT_OUTPUT_INT(511);
}

TEST_P(qrvm, clz_stack_underflow)
{
    execute(bytecode{OP_CLZ});
    EXPECT_STATUS(QRVMC_STACK_UNDERFLOW);
}
