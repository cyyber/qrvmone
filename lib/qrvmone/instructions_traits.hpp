// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "instructions_opcodes.hpp"
#include <array>
#include <optional>

namespace qrvmone::instr
{
/// The special gas cost value marking an QRVM instruction as "undefined".
constexpr int16_t undefined = -1;

/// EIP-2929 constants (https://eips.ethereum.org/EIPS/eip-2929).
/// @{
inline constexpr auto cold_sload_cost = 2100;
inline constexpr auto cold_account_access_cost = 2600;
inline constexpr auto warm_storage_read_cost = 100;

/// Additional cold account access cost.
///
/// The warm access cost is unconditionally applied for every account access instruction.
/// If the access turns out to be cold, this cost must be applied additionally.
inline constexpr auto additional_cold_account_access_cost =
    cold_account_access_cost - warm_storage_read_cost;
/// @}


/// The table of instruction gas costs per QRVM revision.
using GasCostTable = std::array<std::array<int16_t, 256>, QRVMC_MAX_REVISION + 1>;

/// The QRVM revision specific table of QRVM instructions gas costs. For instructions undefined
/// in given QRVM revision, the value is instr::undefined.
constexpr inline GasCostTable gas_costs = []() noexcept {
    GasCostTable table{};

    for (auto& t : table[QRVMC_ZOND])
        t = undefined;
    table[QRVMC_ZOND][OP_STOP] = 0;
    table[QRVMC_ZOND][OP_ADD] = 3;
    table[QRVMC_ZOND][OP_MUL] = 5;
    table[QRVMC_ZOND][OP_SUB] = 3;
    table[QRVMC_ZOND][OP_DIV] = 5;
    table[QRVMC_ZOND][OP_SDIV] = 5;
    table[QRVMC_ZOND][OP_MOD] = 5;
    table[QRVMC_ZOND][OP_SMOD] = 5;
    table[QRVMC_ZOND][OP_ADDMOD] = 8;
    table[QRVMC_ZOND][OP_MULMOD] = 8;
    table[QRVMC_ZOND][OP_EXP] = 10;
    table[QRVMC_ZOND][OP_SIGNEXTEND] = 5;
    table[QRVMC_ZOND][OP_LT] = 3;
    table[QRVMC_ZOND][OP_GT] = 3;
    table[QRVMC_ZOND][OP_SLT] = 3;
    table[QRVMC_ZOND][OP_SGT] = 3;
    table[QRVMC_ZOND][OP_EQ] = 3;
    table[QRVMC_ZOND][OP_ISZERO] = 3;
    table[QRVMC_ZOND][OP_AND] = 3;
    table[QRVMC_ZOND][OP_OR] = 3;
    table[QRVMC_ZOND][OP_XOR] = 3;
    table[QRVMC_ZOND][OP_NOT] = 3;
    table[QRVMC_ZOND][OP_BYTE] = 3;
    table[QRVMC_ZOND][OP_KECCAK256] = 30;
    table[QRVMC_ZOND][OP_ADDRESS] = 2;
    table[QRVMC_ZOND][OP_BALANCE] = warm_storage_read_cost;
    table[QRVMC_ZOND][OP_ORIGIN] = 2;
    table[QRVMC_ZOND][OP_CALLER] = 2;
    table[QRVMC_ZOND][OP_CALLVALUE] = 2;
    table[QRVMC_ZOND][OP_CALLDATALOAD] = 3;
    table[QRVMC_ZOND][OP_CALLDATASIZE] = 2;
    table[QRVMC_ZOND][OP_CALLDATACOPY] = 3;
    table[QRVMC_ZOND][OP_CODESIZE] = 2;
    table[QRVMC_ZOND][OP_CODECOPY] = 3;
    table[QRVMC_ZOND][OP_GASPRICE] = 2;
    table[QRVMC_ZOND][OP_EXTCODESIZE] = warm_storage_read_cost;
    table[QRVMC_ZOND][OP_EXTCODECOPY] = warm_storage_read_cost;
    table[QRVMC_ZOND][OP_BLOCKHASH] = 20;
    table[QRVMC_ZOND][OP_COINBASE] = 2;
    table[QRVMC_ZOND][OP_TIMESTAMP] = 2;
    table[QRVMC_ZOND][OP_NUMBER] = 2;
    table[QRVMC_ZOND][OP_PREVRANDAO] = 2;
    table[QRVMC_ZOND][OP_GASLIMIT] = 2;
    table[QRVMC_ZOND][OP_POP] = 2;
    table[QRVMC_ZOND][OP_MLOAD] = 3;
    table[QRVMC_ZOND][OP_MSTORE] = 3;
    table[QRVMC_ZOND][OP_MSTORE8] = 3;
    table[QRVMC_ZOND][OP_SLOAD] = warm_storage_read_cost;
    table[QRVMC_ZOND][OP_SSTORE] = 0;
    table[QRVMC_ZOND][OP_JUMP] = 8;
    table[QRVMC_ZOND][OP_JUMPI] = 10;
    table[QRVMC_ZOND][OP_PC] = 2;
    table[QRVMC_ZOND][OP_MSIZE] = 2;
    table[QRVMC_ZOND][OP_GAS] = 2;
    table[QRVMC_ZOND][OP_JUMPDEST] = 1;
    for (auto op = size_t{OP_PUSH1}; op <= OP_PUSH64; ++op)
        table[QRVMC_ZOND][op] = 3;
    for (auto op = size_t{OP_DUP1}; op <= OP_DUP16; ++op)
        table[QRVMC_ZOND][op] = 3;
    for (auto op = size_t{OP_SWAP1}; op <= OP_SWAP16; ++op)
        table[QRVMC_ZOND][op] = 3;
    for (auto op = size_t{OP_LOG0}; op <= OP_LOG4; ++op)
        table[QRVMC_ZOND][op] = static_cast<int16_t>((op - OP_LOG0 + 1) * 375);
    table[QRVMC_ZOND][OP_CREATE] = 32000;
    table[QRVMC_ZOND][OP_CALL] = warm_storage_read_cost;
    table[QRVMC_ZOND][OP_RETURN] = 0;
    table[QRVMC_ZOND][OP_INVALID] = 0;
    table[QRVMC_ZOND][OP_DELEGATECALL] = warm_storage_read_cost;
    table[QRVMC_ZOND][OP_RETURNDATASIZE] = 2;
    table[QRVMC_ZOND][OP_RETURNDATACOPY] = 3;
    table[QRVMC_ZOND][OP_STATICCALL] = warm_storage_read_cost;
    table[QRVMC_ZOND][OP_REVERT] = 0;
    table[QRVMC_ZOND][OP_SHL] = 3;
    table[QRVMC_ZOND][OP_SHR] = 3;
    table[QRVMC_ZOND][OP_SAR] = 3;
    table[QRVMC_ZOND][OP_EXTCODEHASH] = warm_storage_read_cost;
    table[QRVMC_ZOND][OP_CREATE2] = 32000;
    table[QRVMC_ZOND][OP_CHAINID] = 2;
    table[QRVMC_ZOND][OP_SELFBALANCE] = 5;
    table[QRVMC_ZOND][OP_BASEFEE] = 2;
    table[QRVMC_ZOND][OP_PUSH0] = 2;

    return table;
}();

static_assert(gas_costs[QRVMC_MAX_REVISION][OP_ADD] > 0, "gas costs missing for a revision");


/// The QRVM instruction traits.
struct Traits
{
    /// The instruction name;
    const char* name = nullptr;

    /// Size of the immediate argument in bytes.
    uint8_t immediate_size = 0;

    /// Whether the instruction terminates execution.
    /// This is false for undefined instructions but this can be changed if desired.
    bool is_terminating = false;

    /// The number of stack items the instruction accesses during execution.
    int8_t stack_height_required = 0;

    /// The stack height change caused by the instruction execution. Can be negative.
    int8_t stack_height_change = 0;

    /// The QRVM revision in which the instruction has been defined. For instructions available in
    /// every QRVM revision the value is ::QRVMC_ZOND. For undefined instructions the value is not
    /// available.
    std::optional<qrvmc_revision> since;
};

// NOTE(rgeraldes24): returns always true for now
/// Determines if an instruction has constant base gas cost across all revisions.
/// Note that this is not true for instructions with constant base gas cost but
/// not available in the first revision (e.g. SHL).
inline constexpr bool has_const_gas_cost(Opcode op) noexcept
{
    const auto g = gas_costs[QRVMC_ZOND][op];
    for (size_t r = QRVMC_ZOND + 1; r <= QRVMC_MAX_REVISION; ++r)
    {
        if (gas_costs[r][op] != g)
            return false;
    }
    return true;
}


/// The global, QRVM revision independent, table of traits of all known QRVM instructions.
constexpr inline std::array<Traits, 256> traits = []() noexcept {
    std::array<Traits, 256> table{};

    table[OP_STOP] = {"STOP", 0, true, 0, 0, QRVMC_ZOND};
    table[OP_ADD] = {"ADD", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_MUL] = {"MUL", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SUB] = {"SUB", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_DIV] = {"DIV", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SDIV] = {"SDIV", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_MOD] = {"MOD", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SMOD] = {"SMOD", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_ADDMOD] = {"ADDMOD", 0, false, 3, -2, QRVMC_ZOND};
    table[OP_MULMOD] = {"MULMOD", 0, false, 3, -2, QRVMC_ZOND};
    table[OP_EXP] = {"EXP", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SIGNEXTEND] = {"SIGNEXTEND", 0, false, 2, -1, QRVMC_ZOND};

    table[OP_LT] = {"LT", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_GT] = {"GT", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SLT] = {"SLT", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SGT] = {"SGT", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_EQ] = {"EQ", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_ISZERO] = {"ISZERO", 0, false, 1, 0, QRVMC_ZOND};
    table[OP_AND] = {"AND", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_OR] = {"OR", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_XOR] = {"XOR", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_NOT] = {"NOT", 0, false, 1, 0, QRVMC_ZOND};
    table[OP_BYTE] = {"BYTE", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SHL] = {"SHL", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SHR] = {"SHR", 0, false, 2, -1, QRVMC_ZOND};
    table[OP_SAR] = {"SAR", 0, false, 2, -1, QRVMC_ZOND};

    table[OP_KECCAK256] = {"KECCAK256", 0, false, 2, -1, QRVMC_ZOND};

    table[OP_ADDRESS] = {"ADDRESS", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_BALANCE] = {"BALANCE", 0, false, 1, 0, QRVMC_ZOND};
    table[OP_ORIGIN] = {"ORIGIN", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_CALLER] = {"CALLER", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_CALLVALUE] = {"CALLVALUE", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_CALLDATALOAD] = {"CALLDATALOAD", 0, false, 1, 0, QRVMC_ZOND};
    table[OP_CALLDATASIZE] = {"CALLDATASIZE", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_CALLDATACOPY] = {"CALLDATACOPY", 0, false, 3, -3, QRVMC_ZOND};
    table[OP_CODESIZE] = {"CODESIZE", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_CODECOPY] = {"CODECOPY", 0, false, 3, -3, QRVMC_ZOND};
    table[OP_GASPRICE] = {"GASPRICE", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_EXTCODESIZE] = {"EXTCODESIZE", 0, false, 1, 0, QRVMC_ZOND};
    table[OP_EXTCODECOPY] = {"EXTCODECOPY", 0, false, 4, -4, QRVMC_ZOND};
    table[OP_RETURNDATASIZE] = {"RETURNDATASIZE", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_RETURNDATACOPY] = {"RETURNDATACOPY", 0, false, 3, -3, QRVMC_ZOND};
    table[OP_EXTCODEHASH] = {"EXTCODEHASH", 0, false, 1, 0, QRVMC_ZOND};

    table[OP_BLOCKHASH] = {"BLOCKHASH", 0, false, 1, 0, QRVMC_ZOND};
    table[OP_COINBASE] = {"COINBASE", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_TIMESTAMP] = {"TIMESTAMP", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_NUMBER] = {"NUMBER", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_PREVRANDAO] = {"PREVRANDAO", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_GASLIMIT] = {"GASLIMIT", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_CHAINID] = {"CHAINID", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_SELFBALANCE] = {"SELFBALANCE", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_BASEFEE] = {"BASEFEE", 0, false, 0, 1, QRVMC_ZOND};

    table[OP_POP] = {"POP", 0, false, 1, -1, QRVMC_ZOND};
    table[OP_MLOAD] = {"MLOAD", 0, false, 1, 0, QRVMC_ZOND};
    table[OP_MSTORE] = {"MSTORE", 0, false, 2, -2, QRVMC_ZOND};
    table[OP_MSTORE8] = {"MSTORE8", 0, false, 2, -2, QRVMC_ZOND};
    table[OP_SLOAD] = {"SLOAD", 0, false, 1, 0, QRVMC_ZOND};
    table[OP_SSTORE] = {"SSTORE", 0, false, 2, -2, QRVMC_ZOND};
    table[OP_JUMP] = {"JUMP", 0, false, 1, -1, QRVMC_ZOND};
    table[OP_JUMPI] = {"JUMPI", 0, false, 2, -2, QRVMC_ZOND};
    table[OP_PC] = {"PC", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_MSIZE] = {"MSIZE", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_GAS] = {"GAS", 0, false, 0, 1, QRVMC_ZOND};
    table[OP_JUMPDEST] = {"JUMPDEST", 0, false, 0, 0, QRVMC_ZOND};

    table[OP_PUSH0] = {"PUSH0", 0, false, 0, 1, QRVMC_ZOND};

    table[OP_PUSH1] = {"PUSH1", 1, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH2] = {"PUSH2", 2, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH3] = {"PUSH3", 3, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH4] = {"PUSH4", 4, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH5] = {"PUSH5", 5, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH6] = {"PUSH6", 6, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH7] = {"PUSH7", 7, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH8] = {"PUSH8", 8, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH9] = {"PUSH9", 9, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH10] = {"PUSH10", 10, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH11] = {"PUSH11", 11, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH12] = {"PUSH12", 12, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH13] = {"PUSH13", 13, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH14] = {"PUSH14", 14, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH15] = {"PUSH15", 15, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH16] = {"PUSH16", 16, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH17] = {"PUSH17", 17, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH18] = {"PUSH18", 18, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH19] = {"PUSH19", 19, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH20] = {"PUSH20", 20, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH21] = {"PUSH21", 21, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH22] = {"PUSH22", 22, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH23] = {"PUSH23", 23, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH24] = {"PUSH24", 24, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH25] = {"PUSH25", 25, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH26] = {"PUSH26", 26, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH27] = {"PUSH27", 27, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH28] = {"PUSH28", 28, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH29] = {"PUSH29", 29, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH30] = {"PUSH30", 30, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH31] = {"PUSH31", 31, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH32] = {"PUSH32", 32, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH33] = {"PUSH33", 33, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH34] = {"PUSH34", 34, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH35] = {"PUSH35", 35, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH36] = {"PUSH36", 36, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH37] = {"PUSH37", 37, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH38] = {"PUSH38", 38, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH39] = {"PUSH39", 39, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH40] = {"PUSH40", 40, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH41] = {"PUSH41", 41, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH42] = {"PUSH42", 42, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH43] = {"PUSH43", 43, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH44] = {"PUSH44", 44, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH45] = {"PUSH45", 45, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH46] = {"PUSH46", 46, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH47] = {"PUSH47", 47, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH48] = {"PUSH48", 48, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH49] = {"PUSH49", 49, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH50] = {"PUSH50", 50, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH51] = {"PUSH51", 51, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH52] = {"PUSH52", 52, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH53] = {"PUSH53", 53, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH54] = {"PUSH54", 54, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH55] = {"PUSH55", 55, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH56] = {"PUSH56", 56, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH57] = {"PUSH57", 57, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH58] = {"PUSH58", 58, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH59] = {"PUSH59", 59, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH60] = {"PUSH60", 60, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH61] = {"PUSH61", 61, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH62] = {"PUSH62", 62, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH63] = {"PUSH63", 63, false, 0, 1, QRVMC_ZOND};
    table[OP_PUSH64] = {"PUSH64", 64, false, 0, 1, QRVMC_ZOND};

    table[OP_DUP1] = {"DUP1", 0, false, 1, 1, QRVMC_ZOND};
    table[OP_DUP2] = {"DUP2", 0, false, 2, 1, QRVMC_ZOND};
    table[OP_DUP3] = {"DUP3", 0, false, 3, 1, QRVMC_ZOND};
    table[OP_DUP4] = {"DUP4", 0, false, 4, 1, QRVMC_ZOND};
    table[OP_DUP5] = {"DUP5", 0, false, 5, 1, QRVMC_ZOND};
    table[OP_DUP6] = {"DUP6", 0, false, 6, 1, QRVMC_ZOND};
    table[OP_DUP7] = {"DUP7", 0, false, 7, 1, QRVMC_ZOND};
    table[OP_DUP8] = {"DUP8", 0, false, 8, 1, QRVMC_ZOND};
    table[OP_DUP9] = {"DUP9", 0, false, 9, 1, QRVMC_ZOND};
    table[OP_DUP10] = {"DUP10", 0, false, 10, 1, QRVMC_ZOND};
    table[OP_DUP11] = {"DUP11", 0, false, 11, 1, QRVMC_ZOND};
    table[OP_DUP12] = {"DUP12", 0, false, 12, 1, QRVMC_ZOND};
    table[OP_DUP13] = {"DUP13", 0, false, 13, 1, QRVMC_ZOND};
    table[OP_DUP14] = {"DUP14", 0, false, 14, 1, QRVMC_ZOND};
    table[OP_DUP15] = {"DUP15", 0, false, 15, 1, QRVMC_ZOND};
    table[OP_DUP16] = {"DUP16", 0, false, 16, 1, QRVMC_ZOND};

    table[OP_SWAP1] = {"SWAP1", 0, false, 2, 0, QRVMC_ZOND};
    table[OP_SWAP2] = {"SWAP2", 0, false, 3, 0, QRVMC_ZOND};
    table[OP_SWAP3] = {"SWAP3", 0, false, 4, 0, QRVMC_ZOND};
    table[OP_SWAP4] = {"SWAP4", 0, false, 5, 0, QRVMC_ZOND};
    table[OP_SWAP5] = {"SWAP5", 0, false, 6, 0, QRVMC_ZOND};
    table[OP_SWAP6] = {"SWAP6", 0, false, 7, 0, QRVMC_ZOND};
    table[OP_SWAP7] = {"SWAP7", 0, false, 8, 0, QRVMC_ZOND};
    table[OP_SWAP8] = {"SWAP8", 0, false, 9, 0, QRVMC_ZOND};
    table[OP_SWAP9] = {"SWAP9", 0, false, 10, 0, QRVMC_ZOND};
    table[OP_SWAP10] = {"SWAP10", 0, false, 11, 0, QRVMC_ZOND};
    table[OP_SWAP11] = {"SWAP11", 0, false, 12, 0, QRVMC_ZOND};
    table[OP_SWAP12] = {"SWAP12", 0, false, 13, 0, QRVMC_ZOND};
    table[OP_SWAP13] = {"SWAP13", 0, false, 14, 0, QRVMC_ZOND};
    table[OP_SWAP14] = {"SWAP14", 0, false, 15, 0, QRVMC_ZOND};
    table[OP_SWAP15] = {"SWAP15", 0, false, 16, 0, QRVMC_ZOND};
    table[OP_SWAP16] = {"SWAP16", 0, false, 17, 0, QRVMC_ZOND};

    table[OP_LOG0] = {"LOG0", 0, false, 2, -2, QRVMC_ZOND};
    table[OP_LOG1] = {"LOG1", 0, false, 3, -3, QRVMC_ZOND};
    table[OP_LOG2] = {"LOG2", 0, false, 4, -4, QRVMC_ZOND};
    table[OP_LOG3] = {"LOG3", 0, false, 5, -5, QRVMC_ZOND};
    table[OP_LOG4] = {"LOG4", 0, false, 6, -6, QRVMC_ZOND};

    table[OP_CREATE] = {"CREATE", 0, false, 3, -2, QRVMC_ZOND};
    table[OP_CALL] = {"CALL", 0, false, 7, -6, QRVMC_ZOND};
    table[OP_RETURN] = {"RETURN", 0, true, 2, -2, QRVMC_ZOND};
    table[OP_DELEGATECALL] = {"DELEGATECALL", 0, false, 6, -5, QRVMC_ZOND};
    table[OP_CREATE2] = {"CREATE2", 0, false, 4, -3, QRVMC_ZOND};
    table[OP_STATICCALL] = {"STATICCALL", 0, false, 6, -5, QRVMC_ZOND};
    table[OP_REVERT] = {"REVERT", 0, true, 2, -2, QRVMC_ZOND};
    table[OP_INVALID] = {"INVALID", 0, true, 0, 0, QRVMC_ZOND};

    return table;
}();

}  // namespace qrvmone::instr
