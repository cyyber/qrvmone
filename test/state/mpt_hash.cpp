// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2022 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "mpt_hash.hpp"
#include "account.hpp"
#include "mpt.hpp"
#include "rlp.hpp"
#include "state.hpp"

namespace qrvmone::state
{
namespace
{
hash256 mpt_hash(const std::unordered_map<hash256, StorageValue>& storage)
{
    MPT trie;
    for (const auto& [key, value] : storage)
    {
        if (!is_zero(value.current))  // Skip "deleted" values.
            trie.insert(keccak256(key), rlp::encode(rlp::trim(value.current)));
    }
    return trie.hash();
}
}  // namespace

hash256 mpt_hash(const std::unordered_map<address, Account>& accounts)
{
    MPT trie;
    for (const auto& [addr, acc] : accounts)
    {
        // After the 64-byte VM-word migration hash256 (= bytes64) is 64
        // bytes wide with the 32-byte keccak hash sitting in the low
        // half. Account-RLP storage_root/code_hash fields stay 32 bytes
        // wide per the Ethereum spec, so explicitly slice to the low
        // half before RLP-encoding.
        const auto storage_root = mpt_hash(acc.storage);
        const auto code_hash = keccak256(acc.code);
        trie.insert(keccak256(addr),
            rlp::encode_tuple(acc.nonce, acc.balance,
                bytes_view{storage_root.bytes + 32, 32},
                bytes_view{code_hash.bytes + 32, 32}));
    }
    return trie.hash();
}

hash256 mpt_hash(std::span<const Transaction> transactions)
{
    MPT trie;
    for (size_t i = 0; i < transactions.size(); ++i)
        trie.insert(rlp::encode(i), rlp::encode(transactions[i]));
    return trie.hash();
}

hash256 mpt_hash(std::span<const TransactionReceipt> receipts)
{
    MPT trie;
    for (size_t i = 0; i < receipts.size(); ++i)
        trie.insert(rlp::encode(i), rlp::encode(receipts[i]));

    return trie.hash();
}

}  // namespace qrvmone::state
