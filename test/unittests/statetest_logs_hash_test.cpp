// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <test/statetest/statetest.hpp>

using namespace qrvmone;

static constexpr auto EmptyLogsHash =
    0x1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347_bytes32;

TEST(statetest_logs_hash, empty_logs)
{
    EXPECT_EQ(test::logs_hash({}), EmptyLogsHash);
    EXPECT_EQ(keccak256(bytes{0xc0}), EmptyLogsHash);  // Hash of empty RLP list: 0xc0.
}

TEST(statetest_logs_hash, example1)
{
    const std::vector<state::Log> logs{
        state::Log{"Q00"_address, bytes{0xb0, 0xb1}, {}},
        state::Log{"Qaa"_address, {}, {0x01_bytes32, 0x02_bytes32}},
    };

    // Expected hash regenerated post-migration: log addresses are now 48
    // bytes wide so the RLP-encoded log entry differs from the
    // pre-migration 20-byte form.
    EXPECT_EQ(test::logs_hash(logs),
        0xc7d23e46643e1907ee7ef59549f9c90600ede14366def1a393499cfa4108749c_bytes32);
}
