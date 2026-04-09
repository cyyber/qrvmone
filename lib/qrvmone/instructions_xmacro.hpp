// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2022 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

/// The default macro for ON_OPCODE_IDENTIFIER. It redirects to ON_OPCODE.
#define ON_OPCODE_IDENTIFIER_DEFAULT(OPCODE, NAME) ON_OPCODE(OPCODE)

/// The default macro for ON_OPCODE_UNDEFINED. Empty implementation to ignore undefined opcodes.
#define ON_OPCODE_UNDEFINED_DEFAULT(OPCODE)


#define ON_OPCODE_IDENTIFIER ON_OPCODE_IDENTIFIER_DEFAULT
#define ON_OPCODE_UNDEFINED ON_OPCODE_UNDEFINED_DEFAULT


/// The "X Macro" for opcodes and their matching identifiers.
///
/// The MAP_OPCODES is an extended variant of X Macro idiom.
/// It has 3 knobs for users.
///
/// 1. The ON_OPCODE(OPCODE) macro must be defined. It will receive all defined opcodes from
///    the qrvmc_opcode enum.
/// 2. The ON_OPCODE_UNDEFINED(OPCODE) macro may be defined to receive
///    the values of all undefined opcodes.
///    This macro is by default alias to ON_OPCODE_UNDEFINED_DEFAULT therefore users must first
///    undef it and restore the alias after usage.
/// 3. The ON_OPCODE_IDENTIFIER(OPCODE, IDENTIFIER) macro may be defined to receive
///    the pairs of all defined opcodes and their matching identifiers.
///    This macro is by default alias to ON_OPCODE_IDENTIFIER_DEFAULT therefore users must first
///    undef it and restore the alias after usage.
///
/// See for more about X Macros: https://en.wikipedia.org/wiki/X_Macro.
#define MAP_OPCODES                                         \
    ON_OPCODE_IDENTIFIER(OP_STOP, stop)                     \
    ON_OPCODE_IDENTIFIER(OP_ADD, add)                       \
    ON_OPCODE_IDENTIFIER(OP_MUL, mul)                       \
    ON_OPCODE_IDENTIFIER(OP_SUB, sub)                       \
    ON_OPCODE_IDENTIFIER(OP_DIV, div)                       \
    ON_OPCODE_IDENTIFIER(OP_SDIV, sdiv)                     \
    ON_OPCODE_IDENTIFIER(OP_MOD, mod)                       \
    ON_OPCODE_IDENTIFIER(OP_SMOD, smod)                     \
    ON_OPCODE_IDENTIFIER(OP_ADDMOD, addmod)                 \
    ON_OPCODE_IDENTIFIER(OP_MULMOD, mulmod)                 \
    ON_OPCODE_IDENTIFIER(OP_EXP, exp)                       \
    ON_OPCODE_IDENTIFIER(OP_SIGNEXTEND, signextend)         \
    ON_OPCODE_UNDEFINED(0x0c)                               \
    ON_OPCODE_UNDEFINED(0x0d)                               \
    ON_OPCODE_UNDEFINED(0x0e)                               \
    ON_OPCODE_UNDEFINED(0x0f)                               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_LT, lt)                         \
    ON_OPCODE_IDENTIFIER(OP_GT, gt)                         \
    ON_OPCODE_IDENTIFIER(OP_SLT, slt)                       \
    ON_OPCODE_IDENTIFIER(OP_SGT, sgt)                       \
    ON_OPCODE_IDENTIFIER(OP_EQ, eq)                         \
    ON_OPCODE_IDENTIFIER(OP_ISZERO, iszero)                 \
    ON_OPCODE_IDENTIFIER(OP_AND, and_)                      \
    ON_OPCODE_IDENTIFIER(OP_OR, or_)                        \
    ON_OPCODE_IDENTIFIER(OP_XOR, xor_)                      \
    ON_OPCODE_IDENTIFIER(OP_NOT, not_)                      \
    ON_OPCODE_IDENTIFIER(OP_BYTE, byte)                     \
    ON_OPCODE_IDENTIFIER(OP_SHL, shl)                       \
    ON_OPCODE_IDENTIFIER(OP_SHR, shr)                       \
    ON_OPCODE_IDENTIFIER(OP_SAR, sar)                       \
    ON_OPCODE_UNDEFINED(0x1e)                               \
    ON_OPCODE_UNDEFINED(0x1f)                               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_KECCAK256, keccak256)           \
    ON_OPCODE_UNDEFINED(0x21)                               \
    ON_OPCODE_UNDEFINED(0x22)                               \
    ON_OPCODE_UNDEFINED(0x23)                               \
    ON_OPCODE_UNDEFINED(0x24)                               \
    ON_OPCODE_UNDEFINED(0x25)                               \
    ON_OPCODE_UNDEFINED(0x26)                               \
    ON_OPCODE_UNDEFINED(0x27)                               \
    ON_OPCODE_UNDEFINED(0x28)                               \
    ON_OPCODE_UNDEFINED(0x29)                               \
    ON_OPCODE_UNDEFINED(0x2a)                               \
    ON_OPCODE_UNDEFINED(0x2b)                               \
    ON_OPCODE_UNDEFINED(0x2c)                               \
    ON_OPCODE_UNDEFINED(0x2d)                               \
    ON_OPCODE_UNDEFINED(0x2e)                               \
    ON_OPCODE_UNDEFINED(0x2f)                               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_ADDRESS, address)               \
    ON_OPCODE_IDENTIFIER(OP_BALANCE, balance)               \
    ON_OPCODE_IDENTIFIER(OP_ORIGIN, origin)                 \
    ON_OPCODE_IDENTIFIER(OP_CALLER, caller)                 \
    ON_OPCODE_IDENTIFIER(OP_CALLVALUE, callvalue)           \
    ON_OPCODE_IDENTIFIER(OP_CALLDATALOAD, calldataload)     \
    ON_OPCODE_IDENTIFIER(OP_CALLDATASIZE, calldatasize)     \
    ON_OPCODE_IDENTIFIER(OP_CALLDATACOPY, calldatacopy)     \
    ON_OPCODE_IDENTIFIER(OP_CODESIZE, codesize)             \
    ON_OPCODE_IDENTIFIER(OP_CODECOPY, codecopy)             \
    ON_OPCODE_IDENTIFIER(OP_GASPRICE, gasprice)             \
    ON_OPCODE_IDENTIFIER(OP_EXTCODESIZE, extcodesize)       \
    ON_OPCODE_IDENTIFIER(OP_EXTCODECOPY, extcodecopy)       \
    ON_OPCODE_IDENTIFIER(OP_RETURNDATASIZE, returndatasize) \
    ON_OPCODE_IDENTIFIER(OP_RETURNDATACOPY, returndatacopy) \
    ON_OPCODE_IDENTIFIER(OP_EXTCODEHASH, extcodehash)       \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_BLOCKHASH, blockhash)           \
    ON_OPCODE_IDENTIFIER(OP_COINBASE, coinbase)             \
    ON_OPCODE_IDENTIFIER(OP_TIMESTAMP, timestamp)           \
    ON_OPCODE_IDENTIFIER(OP_NUMBER, number)                 \
    ON_OPCODE_IDENTIFIER(OP_PREVRANDAO, prevrandao)         \
    ON_OPCODE_IDENTIFIER(OP_GASLIMIT, gaslimit)             \
    ON_OPCODE_IDENTIFIER(OP_CHAINID, chainid)               \
    ON_OPCODE_IDENTIFIER(OP_SELFBALANCE, selfbalance)       \
    ON_OPCODE_IDENTIFIER(OP_BASEFEE, basefee)               \
    ON_OPCODE_UNDEFINED(0x49)                               \
    ON_OPCODE_UNDEFINED(0x4a)                               \
    ON_OPCODE_UNDEFINED(0x4b)                               \
    ON_OPCODE_UNDEFINED(0x4c)                               \
    ON_OPCODE_UNDEFINED(0x4d)                               \
    ON_OPCODE_UNDEFINED(0x4e)                               \
    ON_OPCODE_UNDEFINED(0x4f)                               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_POP, pop)                       \
    ON_OPCODE_IDENTIFIER(OP_MLOAD, mload)                   \
    ON_OPCODE_IDENTIFIER(OP_MSTORE, mstore)                 \
    ON_OPCODE_IDENTIFIER(OP_MSTORE8, mstore8)               \
    ON_OPCODE_IDENTIFIER(OP_SLOAD, sload)                   \
    ON_OPCODE_IDENTIFIER(OP_SSTORE, sstore)                 \
    ON_OPCODE_IDENTIFIER(OP_JUMP, jump)                     \
    ON_OPCODE_IDENTIFIER(OP_JUMPI, jumpi)                   \
    ON_OPCODE_IDENTIFIER(OP_PC, pc)                         \
    ON_OPCODE_IDENTIFIER(OP_MSIZE, msize)                   \
    ON_OPCODE_IDENTIFIER(OP_GAS, gas)                       \
    ON_OPCODE_IDENTIFIER(OP_JUMPDEST, jumpdest)             \
    ON_OPCODE_UNDEFINED(0x5c)                               \
    ON_OPCODE_UNDEFINED(0x5d)                               \
    ON_OPCODE_UNDEFINED(0x5e)                               \
    ON_OPCODE_IDENTIFIER(OP_PUSH0, push0)                   \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_PUSH1, push<1>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH2, push<2>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH3, push<3>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH4, push<4>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH5, push<5>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH6, push<6>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH7, push<7>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH8, push<8>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH9, push<9>)                 \
    ON_OPCODE_IDENTIFIER(OP_PUSH10, push<10>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH11, push<11>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH12, push<12>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH13, push<13>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH14, push<14>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH15, push<15>)               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_PUSH16, push<16>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH17, push<17>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH18, push<18>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH19, push<19>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH20, push<20>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH21, push<21>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH22, push<22>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH23, push<23>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH24, push<24>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH25, push<25>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH26, push<26>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH27, push<27>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH28, push<28>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH29, push<29>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH30, push<30>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH31, push<31>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH32, push<32>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH33, push<33>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH34, push<34>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH35, push<35>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH36, push<36>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH37, push<37>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH38, push<38>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH39, push<39>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH40, push<40>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH41, push<41>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH42, push<42>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH43, push<43>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH44, push<44>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH45, push<45>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH46, push<46>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH47, push<47>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH48, push<48>)               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_PUSH49, push<49>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH50, push<50>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH51, push<51>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH52, push<52>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH53, push<53>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH54, push<54>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH55, push<55>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH56, push<56>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH57, push<57>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH58, push<58>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH59, push<59>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH60, push<60>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH61, push<61>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH62, push<62>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH63, push<63>)               \
    ON_OPCODE_IDENTIFIER(OP_PUSH64, push<64>)               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_DUP1, dup<1>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP2, dup<2>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP3, dup<3>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP4, dup<4>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP5, dup<5>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP6, dup<6>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP7, dup<7>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP8, dup<8>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP9, dup<9>)                   \
    ON_OPCODE_IDENTIFIER(OP_DUP10, dup<10>)                 \
    ON_OPCODE_IDENTIFIER(OP_DUP11, dup<11>)                 \
    ON_OPCODE_IDENTIFIER(OP_DUP12, dup<12>)                 \
    ON_OPCODE_IDENTIFIER(OP_DUP13, dup<13>)                 \
    ON_OPCODE_IDENTIFIER(OP_DUP14, dup<14>)                 \
    ON_OPCODE_IDENTIFIER(OP_DUP15, dup<15>)                 \
    ON_OPCODE_IDENTIFIER(OP_DUP16, dup<16>)                 \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_SWAP1, swap<1>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP2, swap<2>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP3, swap<3>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP4, swap<4>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP5, swap<5>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP6, swap<6>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP7, swap<7>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP8, swap<8>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP9, swap<9>)                 \
    ON_OPCODE_IDENTIFIER(OP_SWAP10, swap<10>)               \
    ON_OPCODE_IDENTIFIER(OP_SWAP11, swap<11>)               \
    ON_OPCODE_IDENTIFIER(OP_SWAP12, swap<12>)               \
    ON_OPCODE_IDENTIFIER(OP_SWAP13, swap<13>)               \
    ON_OPCODE_IDENTIFIER(OP_SWAP14, swap<14>)               \
    ON_OPCODE_IDENTIFIER(OP_SWAP15, swap<15>)               \
    ON_OPCODE_IDENTIFIER(OP_SWAP16, swap<16>)               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_LOG0, log<0>)                   \
    ON_OPCODE_IDENTIFIER(OP_LOG1, log<1>)                   \
    ON_OPCODE_IDENTIFIER(OP_LOG2, log<2>)                   \
    ON_OPCODE_IDENTIFIER(OP_LOG3, log<3>)                   \
    ON_OPCODE_IDENTIFIER(OP_LOG4, log<4>)                   \
    ON_OPCODE_UNDEFINED(0xc5)                               \
    ON_OPCODE_UNDEFINED(0xc6)                               \
    ON_OPCODE_UNDEFINED(0xc7)                               \
    ON_OPCODE_UNDEFINED(0xc8)                               \
    ON_OPCODE_UNDEFINED(0xc9)                               \
    ON_OPCODE_UNDEFINED(0xca)                               \
    ON_OPCODE_UNDEFINED(0xcb)                               \
    ON_OPCODE_UNDEFINED(0xcc)                               \
    ON_OPCODE_UNDEFINED(0xcd)                               \
    ON_OPCODE_UNDEFINED(0xce)                               \
    ON_OPCODE_UNDEFINED(0xcf)                               \
                                                            \
    ON_OPCODE_UNDEFINED(0xd0)                               \
    ON_OPCODE_UNDEFINED(0xd1)                               \
    ON_OPCODE_UNDEFINED(0xd2)                               \
    ON_OPCODE_UNDEFINED(0xd3)                               \
    ON_OPCODE_UNDEFINED(0xd4)                               \
    ON_OPCODE_UNDEFINED(0xd5)                               \
    ON_OPCODE_UNDEFINED(0xd6)                               \
    ON_OPCODE_UNDEFINED(0xd7)                               \
    ON_OPCODE_UNDEFINED(0xd8)                               \
    ON_OPCODE_UNDEFINED(0xd9)                               \
    ON_OPCODE_UNDEFINED(0xda)                               \
    ON_OPCODE_UNDEFINED(0xdb)                               \
    ON_OPCODE_UNDEFINED(0xdc)                               \
    ON_OPCODE_UNDEFINED(0xdd)                               \
    ON_OPCODE_UNDEFINED(0xde)                               \
    ON_OPCODE_UNDEFINED(0xdf)                               \
                                                            \
    ON_OPCODE_UNDEFINED(0xe0)                               \
    ON_OPCODE_UNDEFINED(0xe1)                               \
    ON_OPCODE_UNDEFINED(0xe2)                               \
    ON_OPCODE_UNDEFINED(0xe3)                               \
    ON_OPCODE_UNDEFINED(0xe4)                               \
    ON_OPCODE_UNDEFINED(0xe5)                               \
    ON_OPCODE_UNDEFINED(0xe6)                               \
    ON_OPCODE_UNDEFINED(0xe7)                               \
    ON_OPCODE_UNDEFINED(0xe8)                               \
    ON_OPCODE_UNDEFINED(0xe9)                               \
    ON_OPCODE_UNDEFINED(0xea)                               \
    ON_OPCODE_UNDEFINED(0xeb)                               \
    ON_OPCODE_UNDEFINED(0xec)                               \
    ON_OPCODE_UNDEFINED(0xed)                               \
    ON_OPCODE_UNDEFINED(0xee)                               \
    ON_OPCODE_UNDEFINED(0xef)                               \
                                                            \
    ON_OPCODE_IDENTIFIER(OP_CREATE, create)                 \
    ON_OPCODE_IDENTIFIER(OP_CALL, call)                     \
    ON_OPCODE_UNDEFINED(0xf2)                               \
    ON_OPCODE_IDENTIFIER(OP_RETURN, return_)                \
    ON_OPCODE_IDENTIFIER(OP_DELEGATECALL, delegatecall)     \
    ON_OPCODE_IDENTIFIER(OP_CREATE2, create2)               \
    ON_OPCODE_UNDEFINED(0xf6)                               \
    ON_OPCODE_UNDEFINED(0xf7)                               \
    ON_OPCODE_UNDEFINED(0xf8)                               \
    ON_OPCODE_UNDEFINED(0xf9)                               \
    ON_OPCODE_IDENTIFIER(OP_STATICCALL, staticcall)         \
    ON_OPCODE_UNDEFINED(0xfb)                               \
    ON_OPCODE_UNDEFINED(0xfc)                               \
    ON_OPCODE_IDENTIFIER(OP_REVERT, revert)                 \
    ON_OPCODE_IDENTIFIER(OP_INVALID, invalid)               \
    ON_OPCODE_UNDEFINED(0xff)
