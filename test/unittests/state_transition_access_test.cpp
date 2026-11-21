// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// SPDX-License-Identifier: Apache-2.0

#include "../utils/bytecode.hpp"
#include "state_transition.hpp"

using namespace qrvmc::literals;
using namespace qrvmone::test;

namespace
{
/// The gas used by a transaction to the code "PUSH1 address, BALANCE":
/// 21000 for the transaction, 3 for the push and the cost of the account access.
constexpr int64_t balance_tx_gas_used(int64_t account_access_cost) noexcept
{
    return 21000 + 3 + account_access_cost;
}

constexpr auto warm_account_access_cost = 100;
constexpr auto cold_account_access_cost = 2600;
}  // namespace

// go-qrl has the addresses of its precompiles (vm.ActivePrecompiles(): 0x01 to 0x07)
// in the access list of every transaction, and no other address but the ones
// of the transaction itself.

TEST_F(state_transition, first_precompile_is_warm)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = push(0x01) + OP_BALANCE});
    expect.gas_used = balance_tx_gas_used(warm_account_access_cost);
    expect.post[*tx.to].exists = true;
}

TEST_F(state_transition, last_precompile_is_warm)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = push(0x07) + OP_BALANCE});
    expect.gas_used = balance_tx_gas_used(warm_account_access_cost);
    expect.post[*tx.to].exists = true;
}

TEST_F(state_transition, address_after_precompiles_is_cold)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = push(0x08) + OP_BALANCE});
    expect.gas_used = balance_tx_gas_used(cold_account_access_cost);
    expect.post[*tx.to].exists = true;
}

TEST_F(state_transition, address_0x09_is_cold)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = push(0x09) + OP_BALANCE});
    expect.gas_used = balance_tx_gas_used(cold_account_access_cost);
    expect.post[*tx.to].exists = true;
}

TEST_F(state_transition, address_before_precompiles_is_cold)
{
    tx.to = To;
    pre.insert(*tx.to, {.code = push(0x00) + OP_BALANCE});
    expect.gas_used = balance_tx_gas_used(cold_account_access_cost);
    expect.post[*tx.to].exists = true;
}
