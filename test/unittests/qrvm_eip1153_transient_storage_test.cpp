// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// SPDX-License-Identifier: Apache-2.0

/// This file contains QRVM unit tests for EIP-1153 "Transient storage opcodes"
/// https://eips.ethereum.org/EIPS/eip-1153

#include "qrvm_fixture.hpp"
#include <string>

using namespace qrvmc::literals;
using qrvmone::test::qrvm;

namespace
{
// The pieces of go-qrl's conformance vectors (core/vm/conformance/vectors.go at
// cyyber/go-qrl ee7ada2), so that both runtimes assert the same results and gas.

/// MSTORE of the top of the stack at 0 and RETURN of those 64 bytes: 15 gas.
const std::string store_and_return_64 = "60005260406000f3";

/// PUSH1 0x2a.
const std::string push_42 = "602a";

/// PUSH1 5: the key.
const std::string push_key = "6005";

/// PUSH33 2**256 + 5: the same key with a bit set above its low 256 bits.
const std::string push_key_above_2_256 = "8001" + std::string(62, '0') + "05";

/// PUSH64 of the bytes 0x00 to 0x3f.
const std::string push_64_bytes =
    "9f"
    "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
    "202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f";

constexpr auto key_5 = 0x05_bytes64;
}  // namespace

TEST_P(qrvm, tstore_tload_round_trip)
{
    execute(100000, bytecode{push_42 + push_key + "5d" + push_key + "5c" + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 224);
    ASSERT_EQ(result.output_size, 64);
    EXPECT_EQ(output, bytes(63, 0) + bytes{0x2a});

    const auto& account = host.accounts[msg.recipient];
    ASSERT_EQ(account.transient_storage.size(), 1);
    EXPECT_EQ(account.transient_storage.at(key_5), 0x2a_bytes64);
    EXPECT_TRUE(account.storage.empty());
}

TEST_P(qrvm, tload_unset_key)
{
    execute(100000, bytecode{push_key + "5c" + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 118);
    EXPECT_EQ(output, bytes(64, 0));
    // Reading does not create an entry.
    EXPECT_TRUE(host.accounts[msg.recipient].transient_storage.empty());
}

TEST_P(qrvm, transient_storage_key_aliases_low_256_bits)
{
    // Operands that differ only above bit 255 address the same slot, as for SLOAD and SSTORE.
    execute(100000,
        bytecode{push_42 + push_key_above_2_256 + "5d" + push_key + "5c" + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 224);
    EXPECT_EQ(output, bytes(63, 0) + bytes{0x2a});

    const auto& transient_storage = host.accounts[msg.recipient].transient_storage;
    ASSERT_EQ(transient_storage.size(), 1);
    EXPECT_EQ(transient_storage.begin()->first, key_5);

    execute(100000,
        bytecode{push_42 + push_key + "5d" + push_key_above_2_256 + "5c" + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 224);
    EXPECT_EQ(output, bytes(63, 0) + bytes{0x2a});
    EXPECT_EQ(transient_storage.size(), 1);
}

TEST_P(qrvm, tstore_leaves_storage_untouched)
{
    execute(100000, bytecode{push_42 + push_key + "5d" + push_key + "54" + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 2224);
    EXPECT_EQ(output, bytes(64, 0));
}

TEST_P(qrvm, sstore_leaves_transient_storage_untouched)
{
    execute(100000, bytecode{push_42 + push_key + "55" + push_key + "5c" + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 22224);
    EXPECT_EQ(output, bytes(64, 0));
    EXPECT_TRUE(host.accounts[msg.recipient].transient_storage.empty());
}

TEST_P(qrvm, tstore_tload_64_byte_value)
{
    // The value is a whole 64-byte word: only the key is cut to 256 bits.
    execute(
        100000, bytecode{push_64_bytes + push_key + "5d" + push_key + "5c" + store_and_return_64});
    EXPECT_GAS_USED(QRVMC_SUCCESS, 224);
    ASSERT_EQ(result.output_size, 64);
    for (size_t i = 0; i < 64; ++i)
        EXPECT_EQ(output[i], static_cast<uint8_t>(i)) << i;

    const auto& value = host.accounts[msg.recipient].transient_storage.at(key_5);
    for (size_t i = 0; i < 64; ++i)
        EXPECT_EQ(value.bytes[i], static_cast<uint8_t>(i)) << i;
}

TEST_P(qrvm, tstore_overwrite)
{
    execute(tstore(1, 0xaa) + tstore(2, 0xbb) + tstore(1, 0xcc) + mstore(0, tload(1)) +
            mstore(64, tload(2)) + ret(0, 128));
    EXPECT_STATUS(QRVMC_SUCCESS);
    ASSERT_EQ(result.output_size, 128);
    EXPECT_EQ(output[63], 0xcc);
    EXPECT_EQ(output[127], 0xbb);
    EXPECT_EQ(host.accounts[msg.recipient].transient_storage.size(), 2);
}

TEST_P(qrvm, tstore_cost)
{
    // 100 gas whatever the value was and becomes, without any refund,
    // and with no minimum of gas left to have as SSTORE has (EIP-2200).
    const auto code = tstore(1, 2);
    execute(106, code);
    EXPECT_GAS_USED(QRVMC_SUCCESS, 106);
    EXPECT_EQ(result.gas_refund, 0);

    execute(106, tstore(1, 0));
    EXPECT_GAS_USED(QRVMC_SUCCESS, 106);
    EXPECT_EQ(result.gas_refund, 0);
    EXPECT_EQ(host.accounts[msg.recipient].transient_storage.at(0x01_bytes64), qrvmc::bytes64{});

    execute(105, code);
    EXPECT_STATUS(QRVMC_OUT_OF_GAS);
    EXPECT_EQ(host.accounts[msg.recipient].transient_storage.at(0x01_bytes64), qrvmc::bytes64{});
}

TEST_P(qrvm, tload_cost)
{
    // 100 gas: transient storage has no cold access.
    host.accounts[msg.recipient].transient_storage[0x01_bytes64] = 0x2a_bytes64;
    execute(103, tload(1));
    EXPECT_GAS_USED(QRVMC_SUCCESS, 103);

    execute(102, tload(1));
    EXPECT_STATUS(QRVMC_OUT_OF_GAS);
}

TEST_P(qrvm, tstore_static_mode)
{
    msg.flags = QRVMC_STATIC;
    execute(100000, tstore(1, 2));
    EXPECT_STATUS(QRVMC_STATIC_MODE_VIOLATION);
    EXPECT_TRUE(host.accounts[msg.recipient].transient_storage.empty());
}

TEST_P(qrvm, tload_static_mode)
{
    host.accounts[msg.recipient].transient_storage[0x01_bytes64] = 0x2a_bytes64;
    msg.flags = QRVMC_STATIC;
    execute(tload(1) + ret_top());
    EXPECT_GAS_USED(QRVMC_SUCCESS, 118);
    EXPECT_OUTPUT_INT(0x2a);
}

TEST_P(qrvm, transient_storage_of_recipient)
{
    // The transient storage is the one of the account the code runs for, not the one
    // the code comes from, as for DELEGATECALL.
    msg.recipient = "Qaa"_address;
    msg.code_address = "Qbb"_address;
    execute(tstore(1, 2));
    EXPECT_STATUS(QRVMC_SUCCESS);
    EXPECT_EQ(host.accounts["Qaa"_address].transient_storage.at(0x01_bytes64), 0x02_bytes64);
    EXPECT_FALSE(host.accounts.contains("Qbb"_address));
}

TEST_P(qrvm, tstore_stack_underflow)
{
    execute(bytecode{OP_TSTORE});
    EXPECT_STATUS(QRVMC_STACK_UNDERFLOW);

    execute(push(1) + OP_TSTORE);
    EXPECT_STATUS(QRVMC_STACK_UNDERFLOW);
}

TEST_P(qrvm, tload_stack_underflow)
{
    execute(bytecode{OP_TLOAD});
    EXPECT_STATUS(QRVMC_STACK_UNDERFLOW);
}
