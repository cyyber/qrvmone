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
inline constexpr bytes64 salts[] = {
    0x00_bytes64, 0xe75fb554e433e03763a1560646ee22dcb74e5274b34c5ad644e7c0f619a7e1d0_bytes64};

// Expected addresses regenerated post-migration (64-byte address +
// 64-byte VM word). compute_new_account_address now Keccak-512s the
// sender and stores the full 64-byte hash, so addresses no longer
// collapse to a right-aligned slice.
TEST(state_new_account_address, create)
{
    for (const auto& ic : init_codes)  // Init-code doesn't affect CREATE.
    {
        auto s = senders[0];
        EXPECT_EQ(addr(s, nonces[0], {}, ic),
            "Qced5630d8a39c22c399a6811280a2dda75b84c0b1494294e2de7ae1c98f27d553928bdd90407ce246402d10602b8291a39ba5c9baef8354cdefb686f973f9186"_address);
        EXPECT_EQ(addr(s, nonces[3], {}, ic),
            "Q69db1180c7978be6e5d2c04df0859b06c594efd40d87daa4e02f0ce86413809d4e4a93b90b2dec0e9fd73cbd626fdd71b89648f998caa56f87ddb2feedbdc772"_address);

        s = senders[1];
        EXPECT_EQ(addr(s, nonces[0], {}, ic),
            "Q568092bfaa4ff2135c8214c243931ec7489a31ce8f11bba215f6d46db92290db215dcc8fcc46ba7c53519efe0003f902a60f1c7e1048186aa5f14e28404868e7"_address);
        EXPECT_EQ(addr(s, nonces[1], {}, ic),
            "Q9f7b89c65c75d81c89da70494dff276e46ee969f7b89dbc63f1ef803d6c9156481feb764388317f39a99c0442aa00c5d822146428cd0cbef9b9b9cf2e70b4997"_address);
        EXPECT_EQ(addr(s, nonces[2], {}, ic),
            "Q7cdc4b427b0924a0b1f641cbae50ddf47eb067a173e2a9ae3daa19dc430ddbac4e6aef691cad1412f365686b672872f87d30caf525963965a40674f4793a0dbd"_address);
        EXPECT_EQ(addr(s, nonces[3], {}, ic),
            "Qeb9a5e08b49f24a5720c485ba374fdb01113eac1366103c0c2bfaf96574f42569b6b0a11eb09cb93f5c623617224349c4e2a7f72c26a5a22570133015dd84b97"_address);

        s = senders[2];
        EXPECT_EQ(addr(s, nonces[0], {}, ic),
            "Qd16f4f43475799bdaac380a872a9ff9b8a9f42d6e687cc14892a383b8a20699d699f99153d3b517b6ed2a2d92437f5ee4287df895569e6246c2c9b643c3eb6b9"_address);
        EXPECT_EQ(addr(s, nonces[3], {}, ic),
            "Q5f223a0f43bebe8352bd428d40c13135f3e65a4730add46b157fa80ee4b50f50f4932715317b8dbef2238322037b05fc88b2cbe070cebfc8cc3d0ea84e3ce761"_address);

        const auto beacon_deposit_address =
            addr("Q00000000000000000000000000000000000000000000000000000000b20a608c624Ca5003905aA834De7156C68b2E1d0"_address, 0, {}, ic);
        EXPECT_EQ(beacon_deposit_address,
            "Qa174895dcf754b696dac4f6374009d0563cb1c7e081469f0402e5e698e5b3bb7f73710a6811a4194a2417d4ffff949a942b1566a321aa7ec74ef95ae0ddc4349"_address);
    }
}

TEST(state_new_account_address, create2)
{
    for (const auto n : nonces)  // Nonce doesn't affect CREATE2.
    {
        EXPECT_EQ(addr(senders[0], n, salts[0], init_codes[0]),
            "Qc1bf3eccdaaec4ecbc9bfd49319cf9fdf1268baf33130d2758928c948afffb69727fd11be2ac23f43fc25dc530a779b9b964c9b271d44dcbea79c61fe9b3d792"_address);

        EXPECT_EQ(addr(senders[2], n, salts[0], init_codes[1]),
            "Q09c866242fc6e6a6c411a83ee298db919806b85f4168816b0a2caa62a4fcafffca0f6fe6bb85b62ce687163e8526af1d1cfe66d7ef48d19726f8b0ec7b70a947"_address);

        EXPECT_EQ(addr(senders[1], n, salts[1], init_codes[0]),
            "Qfdfb835e3962ddd4338259e342563d60ec0e29c4d6de7ea3388982e962e2bbd97b5de45809ef7b4acb8e40fdb93b771bc762f47bc4ba0681a9f9e62af6c5d74d"_address);

        EXPECT_EQ(addr(senders[2], n, salts[1], init_codes[1]),
            "Qd6b81aa9d980c4aa75a0c2577371475cf460c60df0bb10ce029e18fd218f63b34bad3f8d1ad370ca833abec0c1f8cd419bcfe75d5909607f498b639ba88e51cb"_address);
    }
}
