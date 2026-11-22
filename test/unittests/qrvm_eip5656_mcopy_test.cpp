// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// SPDX-License-Identifier: Apache-2.0

/// This file contains QRVM unit tests for EIP-5656 "MCOPY - Memory copying instruction"
/// https://eips.ethereum.org/EIPS/eip-5656

#include "qrvm_fixture.hpp"
#include <string>

using qrvmone::test::qrvm;

namespace
{
// The pieces of go-qrl's conformance vectors (core/vm/conformance/vectors.go at
// cyyber/go-qrl 4044cb7), so that both runtimes assert the same results and gas.

/// The bytes 0x00 to 0x3f. No two of them are equal, so a copy that lands one byte off shows.
const std::string ascending_word =
    "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
    "202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f";

/// PUSH64 of them, PUSH1 0, MSTORE: memory[0..64) is the ascending word, for 12 gas.
const std::string mstore_ascending_word = "9f" + ascending_word + "6000" + "52";

/// MSTORE of the top of the stack at 0 and RETURN of those 64 bytes: 15 gas.
const std::string store_and_return_64 = "60005260406000f3";

/// PUSH9 2**64: the first memory offset that does not fit in 64 bits.
const std::string push_2_64 = "68010000000000000000";

/// PUSH33 2**256: an offset whose low 256 bits are all zero.
const std::string push_2_256 = "8001" + std::string(64, '0');

/// PUSH5 2**32: the first size qrvmone takes for out of gas without computing its cost.
const std::string push_2_32 = "640100000000";

/// MCOPY pops the destination first, then the source, then the size.
/// Each argument is a complete push instruction.
std::string mcopy_hex(const std::string& push_dst, const std::string& push_src,
    const std::string& push_size)
{
    return push_size + push_src + push_dst + "5e";
}

/// A 64-byte word with the given value in its last byte.
std::string word_hex(const std::string& last_byte)
{
    return std::string(126, '0') + last_byte;
}
}  // namespace

TEST_P(qrvm, mcopy_word_to_fresh_memory)
{
    execute(100000,
        bytecode{mstore_ascending_word + mcopy_hex("6040", "6000", "6040") + "60406040f3"});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 36);
    EXPECT_EQ(hex(output), ascending_word);
}

TEST_P(qrvm, mcopy_overlapping_destination_after_source)
{
    execute(100000,
        bytecode{mstore_ascending_word + mcopy_hex("6001", "6000", "603f") + "60406000f3"});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 33);
    EXPECT_EQ(hex(output), "00" + ascending_word.substr(0, 126));
}

TEST_P(qrvm, mcopy_overlapping_destination_before_source)
{
    execute(100000,
        bytecode{mstore_ascending_word + mcopy_hex("6000", "6001", "603f") + "60406000f3"});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 33);
    EXPECT_EQ(hex(output), ascending_word.substr(2) + "3f");
}

TEST_P(qrvm, mcopy_overlapping_by_a_word)
{
    // Two words moved up by one, over their own second word. A copy made forwards
    // in place would repeat the first word instead.
    const auto fill = mstore(0, 0xaa) + mstore(64, 0xbb) + mstore(128, 0xcc);
    execute(fill + mcopy(64, 0, 128) + ret(0, 192));
    EXPECT_STATUS(QRVMC_SUCCESS);
    EXPECT_EQ(hex(output), word_hex("aa") + word_hex("aa") + word_hex("bb"));

    // And down by one, over their own first word.
    execute(fill + mcopy(0, 64, 128) + ret(0, 192));
    EXPECT_STATUS(QRVMC_SUCCESS);
    EXPECT_EQ(hex(output), word_hex("bb") + word_hex("cc") + word_hex("cc"));
}

TEST_P(qrvm, mcopy_same_range)
{
    execute(100000, bytecode{mstore_ascending_word} + mcopy(0, 0, 64) + ret(0, 64));
    EXPECT_GAS_USED(QRVMC_SUCCESS, 33);
    EXPECT_EQ(hex(output), ascending_word);
}

TEST_P(qrvm, mcopy_from_past_the_end_of_memory)
{
    // The source range lies past the end of memory: memory grows to cover it
    // and the destination receives zeros.
    execute(100000,
        bytecode{mstore_ascending_word + mcopy_hex("6000", "6040", "6040") + "60406000f3"});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 36);
    EXPECT_EQ(output, bytes(64, 0));
}

TEST_P(qrvm, mcopy_source_bounds_memory)
{
    // One byte read at 64 makes memory two words long though nothing is written there.
    execute(mcopy(0, 64, 1) + OP_MSIZE + ret_top());
    EXPECT_STATUS(QRVMC_SUCCESS);
    EXPECT_OUTPUT_INT(128);

    execute(mcopy(64, 0, 1) + OP_MSIZE + ret_top());
    EXPECT_STATUS(QRVMC_SUCCESS);
    EXPECT_OUTPUT_INT(128);
}

TEST_P(qrvm, mcopy_65_bytes_is_two_words)
{
    // 65 bytes round up to two words, for the copy cost and for MSIZE alike.
    execute(100000, bytecode{mcopy_hex("6000", "6000", "6041") + "59" + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 38);
    EXPECT_EQ(hex(output), word_hex("80"));
}

TEST_P(qrvm, mcopy_zero_size)
{
    // A zero size never touches memory, whatever the offsets: MSIZE is still 0 afterwards.
    for (const auto& push_offset : {push_2_32, push_2_64, push_2_256})
    {
        execute(100000,
            bytecode{mcopy_hex(push_offset, push_offset, "6000") + "59" + store_and_return_64});
        EXPECT_GAS_USED(QRVMC_SUCCESS, 29);
        EXPECT_EQ(output, bytes(64, 0));
    }
}

TEST_P(qrvm, mcopy_cost)
{
    // 9 for the pushes, 3 for MCOPY, 3 for the word copied and 6 for the two words of memory.
    const auto code = mcopy(0, 64, 64);
    execute(21, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 21);

    // One short of the copy cost, with the memory paid for.
    execute(20, code);
    EXPECT_STATUS(QRVMC_OUT_OF_GAS);

    // One short of the memory cost.
    execute(17, code);
    EXPECT_STATUS(QRVMC_OUT_OF_GAS);
}

TEST_P(qrvm, mcopy_out_of_gas)
{
    // An offset or size from 2**32 up is out of gas, in the low 64 bits or above:
    // an offset is not cut to its low bits.
    for (const auto& push_big : {push_2_32, push_2_64, push_2_256})
    {
        execute(100000, bytecode{mcopy_hex(push_big, "6000", "6001")});
        EXPECT_STATUS(QRVMC_OUT_OF_GAS);

        // The source is the higher offset here: it bounds memory too.
        execute(100000, bytecode{mcopy_hex("6000", push_big, "6001")});
        EXPECT_STATUS(QRVMC_OUT_OF_GAS);

        execute(100000, bytecode{mcopy_hex("6000", "6000", push_big)});
        EXPECT_STATUS(QRVMC_OUT_OF_GAS);
    }
}

TEST_P(qrvm, mcopy_static_mode)
{
    // Memory is not state: MCOPY works in a static call.
    msg.flags = QRVMC_STATIC;
    execute(100000, bytecode{mstore_ascending_word} + mcopy(64, 0, 64) + ret(64, 64));
    EXPECT_GAS_USED(QRVMC_SUCCESS, 36);
    EXPECT_EQ(hex(output), ascending_word);
}

TEST_P(qrvm, mcopy_stack_underflow)
{
    execute(bytecode{OP_MCOPY});
    EXPECT_STATUS(QRVMC_STACK_UNDERFLOW);

    execute(push(0) + OP_MCOPY);
    EXPECT_STATUS(QRVMC_STACK_UNDERFLOW);

    execute(push(0) + push(0) + OP_MCOPY);
    EXPECT_STATUS(QRVMC_STACK_UNDERFLOW);
}
