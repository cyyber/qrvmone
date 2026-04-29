// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "../utils/bytecode.hpp"
#include "state_transition.hpp"

using namespace qrvmc::literals;
using namespace qrvmone::test;

TEST_F(state_transition, create2_factory)
{
    // Expected address regenerated post-migration: 48-byte addresses derived
    // via Keccak-512 of (0xff || sender || salt || keccak512(initcode)).
    static constexpr auto create_address =
        "Q20ccafb9c985640c3a81f1a53f4e165b91bf5fd6637e8cc03b9b370ca22cdd49277a12d8198d1464e8cf95b6560f5d2b"_address;

    const auto factory_code =
        calldatacopy(0, 0, calldatasize()) + create2().input(0, calldatasize());
    const auto initcode = mstore8(0, push(0xFE)) + ret(0, 1);

    tx.to = To;
    tx.data = initcode;
    pre.insert(*tx.to, {.nonce = 1, .code = factory_code});

    expect.post[*tx.to].nonce = pre.get(*tx.to).nonce + 1;  // CREATE caller's nonce must be bumped
    expect.post[create_address].code = bytes{0xFE};
}

TEST_F(state_transition, create_tx)
{
    // Expected address regenerated post-migration: 48-byte addresses derived
    // via Keccak-512(rlp(sender, nonce)).
    static constexpr auto create_address =
        "Q1042769a81a2aa6583882f886c74339b6271a43253dfb7b6c07d3c09e88fa4970ae5116bb2093c154784cbd9dfa1a8fe"_address;

    tx.data = mstore8(0, push(0xFE)) + ret(0, 1);

    expect.post[create_address].code = bytes{0xFE};
}
