// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// SPDX-License-Identifier: Apache-2.0

/// State transition tests of the transient storage (EIP-1153): it is shared by all the
/// call frames of an account in a transaction, reverted with a call that does not succeed
/// and empty again in the next transaction. The fixture checks after every transaction
/// that no account has any transient storage left.

#include "../utils/bytecode.hpp"
#include "state_transition.hpp"

using namespace qrvmc::literals;
using namespace qrvmone::test;

namespace
{
constexpr auto Other = "Q0700e4"_address;
}

TEST_F(state_transition, tstore_tload)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = tstore(1, 0x2a) + sstore(0, tload(1))});
    // 21000 for the transaction, 106 for the TSTORE and its two pushes, 103 for the TLOAD
    // and its push, 22103 for the SSTORE of a new value to a cold slot and its push.
    expect.gas_used = 21000 + 106 + 103 + 22103;
    expect.post[*tx.to].storage[0x00_bytes64] = 0x2a_bytes64;
}

TEST_F(state_transition, transient_storage_seen_by_reentrant_call)
{
    // The use EIP-1153 is made for: the outer call leaves a value which a call back into
    // the same account finds.
    const auto outer = tstore(1, 0x2a) + call(Other).gas(0xffffff) + OP_STOP;
    const auto inner = OP_JUMPDEST + sstore(0, tload(1));
    const auto inner_offset = jumpi(push(0), calldatasize()).size() + outer.size();

    tx.to = To;
    pre.insert(*tx.to, {.code = jumpi(push(inner_offset), calldatasize()) + outer + inner});
    pre.insert(Other, {.code = call(To).gas(0xfffff).input(0, 1)});
    expect.post[*tx.to].storage[0x00_bytes64] = 0x2a_bytes64;
    expect.post[Other].exists = true;
}

TEST_F(state_transition, transient_storage_kept_after_delegatecall)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = tstore(1, 0xaa) + sstore(1, delegatecall(Other).gas(0xffff)) +
                                sstore(0, tload(1))});
    pre.insert(Other, {.code = tstore(1, 0xbb)});
    expect.post[*tx.to].storage[0x00_bytes64] = 0xbb_bytes64;
    expect.post[*tx.to].storage[0x01_bytes64] = 0x01_bytes64;
    expect.post[Other].exists = true;
}

TEST_F(state_transition, transient_storage_reverted_with_call)
{
    // The delegated code sets To's transient storage and then reverts: the value from
    // before the call is back, not zero.
    tx.to = To;
    pre.insert(
        *tx.to, {.code = tstore(1, 0xaa) + sstore(1, add(1, delegatecall(Other).gas(0xffff))) +
                         sstore(0, tload(1))});
    pre.insert(Other, {.code = tstore(1, 0xbb) + revert(0, 0)});
    expect.post[*tx.to].storage[0x00_bytes64] = 0xaa_bytes64;
    expect.post[*tx.to].storage[0x01_bytes64] = 0x01_bytes64;  // 1 + the call's failure.
    expect.post[Other].exists = true;
}

TEST_F(state_transition, transient_storage_reverted_with_failed_call)
{
    // The same for a call that fails by something other than REVERT.
    tx.to = To;
    pre.insert(
        *tx.to, {.code = tstore(1, 0xaa) + sstore(1, add(1, delegatecall(Other).gas(0xffff))) +
                         sstore(0, tload(1))});
    pre.insert(Other, {.code = tstore(1, 0xbb) + OP_INVALID});
    expect.post[*tx.to].storage[0x00_bytes64] = 0xaa_bytes64;
    expect.post[*tx.to].storage[0x01_bytes64] = 0x01_bytes64;
    expect.post[Other].exists = true;
}

TEST_F(state_transition, transient_storage_is_per_account)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = tstore(1, 0xaa) + call(Other).gas(0xffff) + sstore(0, tload(1))});
    // Other does not see To's value at the same key, and its own does not reach To.
    pre.insert(Other, {.code = sstore(0, add(1, tload(1))) + tstore(1, 0xcc)});
    expect.post[*tx.to].storage[0x00_bytes64] = 0xaa_bytes64;
    expect.post[Other].storage[0x00_bytes64] = 0x01_bytes64;
}

TEST_F(state_transition, tstore_in_static_call)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = sstore(1, add(1, staticcall(Other).gas(0xffff)))});
    pre.insert(Other, {.code = tstore(1, 0xbb)});
    expect.post[*tx.to].storage[0x01_bytes64] = 0x01_bytes64;  // 1 + the call's failure.
    expect.post[Other].exists = true;
}

TEST_F(state_transition, transient_storage_reverted_with_transaction)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = tstore(1, 0xaa) + revert(0, 0)});
    expect.status = QRVMC_REVERT;
    expect.post[*tx.to].exists = true;
}

TEST_F(state_transition, transient_storage_empty_in_next_transaction)
{
    // Every run adds one and the transient value it finds to the storage, then leaves
    // a transient value behind. The second transaction must not find it.
    tx.to = To;
    pre.insert(*tx.to, {.code = sstore(0, add(sload(0), add(1, tload(1)))) + tstore(1, 0x10)});
    pre.get(tx.sender).balance += tx.gas_limit * tx.max_gas_price;

    // The first of two equal transactions. The fixture runs the second.
    const auto res = state::transition(pre, block, tx, rev, vm);
    ASSERT_TRUE(holds_alternative<TransactionReceipt>(res));
    EXPECT_EQ(std::get<TransactionReceipt>(res).status, QRVMC_SUCCESS);
    EXPECT_EQ(pre.get(To).storage[0x00_bytes64].current, 0x01_bytes64);
    EXPECT_TRUE(pre.get(To).transient_storage.empty());

    expect.post[*tx.to].storage[0x00_bytes64] = 0x02_bytes64;
}
