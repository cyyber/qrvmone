// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <test/state/host.hpp>

using namespace qrvmc;
using namespace qrvmc::literals;
inline constexpr auto addr = qrvmone::state::compute_new_account_address;

inline constexpr uint64_t nonces[] = {0, 1, 0x80, 0xffffffffffffffff};
inline constexpr address senders[] = {
    "Q00"_address, "Q01"_address, "Q000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000"_address};
inline const bytes init_codes[] = {bytes{}, bytes{0xFE}};
inline constexpr bytes32 salts[] = {
    0x00_bytes32, 0xe75fb554e433e03763a1560646ee22dcb74e5274b34c5ad644e7c0f619a7e1d0_bytes32};

// Expected addresses regenerated post-migration (48-byte address +
// 64-byte VM word). compute_new_account_address now Keccak-512s a
// 48-byte sender and stores the full 48-byte hash, so addresses no
// longer collapse to a 20-byte right-aligned slice.
TEST(state_new_account_address, create)
{
    for (const auto& ic : init_codes)  // Init-code doesn't affect CREATE.
    {
        auto s = senders[0];
        EXPECT_EQ(addr(s, nonces[0], {}, ic),
            "Q25fe345d46568b0059f983193d5cc02122b4620a7ca8703561bf76876c75c9ee93600e16cf9367a20605316d3457c4f0"_address);
        EXPECT_EQ(addr(s, nonces[3], {}, ic),
            "Qb44540e45f7edbd8839aa579c0a480e58aea95a89b0daed81618329a16705546d3d6517379f77b81d060d964aee8b15e"_address);

        s = senders[1];
        EXPECT_EQ(addr(s, nonces[0], {}, ic),
            "Q144723246fe964f18fe38d6c0ea3af7a48c419657e96e0f8982f5e1076205959e36cc36a9d7df70abc677858825cbda4"_address);
        EXPECT_EQ(addr(s, nonces[1], {}, ic),
            "Qc48768787c19605ac0e7809425c8b566e79953fca155c0a2dd06336f4338e379d975004ed0f7a79b1bdb586fcaa611f7"_address);
        EXPECT_EQ(addr(s, nonces[2], {}, ic),
            "Qa1e5d357f66842e526578e27dad85acad14dcfafa8ea290a84249f84c4b0f7a9f047384fc4c08babfe9eb0ae837bc84b"_address);
        EXPECT_EQ(addr(s, nonces[3], {}, ic),
            "Qccdf2e594567d2744c1dc36fb6f02be25fbd0df35c130742c23c6cf3437d338f5c0bec4de9e11afd9edb0ead49fe3bdb"_address);

        s = senders[2];
        EXPECT_EQ(addr(s, nonces[0], {}, ic),
            "Q92b38ae01d6fabaf29d322858859c283e12fb1aaa5f98685ea97c42ee79ee1f2aa9594bf4f6a5e77ac5a842ba31e0f8d"_address);
        EXPECT_EQ(addr(s, nonces[3], {}, ic),
            "Q4b0dd9027ae245110a2ca01ac458a1a8e8e4e156648ee6b89ff0266b923f7811f2e4f98906f3c0a0804610c75010a457"_address);

        const auto beacon_deposit_address =
            addr("Q00000000000000000000000000000000000000000000000000000000b20a608c624Ca5003905aA834De7156C68b2E1d0"_address, 0, {}, ic);
        EXPECT_EQ(beacon_deposit_address,
            "Qa89f8b7459b02d26903258d821dee88a8a6df180775e8f59022a183f4ea1c6a68e32fcc1afedc8172c523f796c0dc187"_address);
    }
}

TEST(state_new_account_address, create2)
{
    for (const auto n : nonces)  // Nonce doesn't affect CREATE2.
    {
        EXPECT_EQ(addr(senders[0], n, salts[0], init_codes[0]),
            "Qd923e645821ccd03fdfb85c299a07a3cc05fb11ae644600a864476b876747adf59bdc1a17bc599def57be18312d0f371"_address);

        EXPECT_EQ(addr(senders[2], n, salts[0], init_codes[1]),
            "Q433c1785f7a277e30faed760bb49ac427d17865892d021e0be23b0686a3d9881bca581dba3ffb8a3f1eeb7ede5ce2611"_address);

        EXPECT_EQ(addr(senders[1], n, salts[1], init_codes[0]),
            "Q6e6085158faae7561efbad4ae9fa6f66047c2b06307a527bb0e9543abc12d2c91d423426c471b475dde1091104c876c5"_address);

        EXPECT_EQ(addr(senders[2], n, salts[1], init_codes[1]),
            "Q87f1026c16c9cec6da704c855195c5067cca8fe8d8aeae1911625eb0086b7a99247c2318647216848e713e922efc1dd6"_address);
    }
}
