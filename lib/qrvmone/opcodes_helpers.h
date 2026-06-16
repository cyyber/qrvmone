// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2019 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#define ANY_SMALL_PUSH \
    OP_PUSH1:          \
    case OP_PUSH2:     \
    case OP_PUSH3:     \
    case OP_PUSH4:     \
    case OP_PUSH5:     \
    case OP_PUSH6:     \
    case OP_PUSH7:     \
    case OP_PUSH8

#define ANY_LARGE_PUSH \
    OP_PUSH9:          \
    case OP_PUSH10:    \
    case OP_PUSH11:    \
    case OP_PUSH12:    \
    case OP_PUSH13:    \
    case OP_PUSH14:    \
    case OP_PUSH15:    \
    case OP_PUSH16:    \
    case OP_PUSH17:    \
    case OP_PUSH18:    \
    case OP_PUSH19:    \
    case OP_PUSH20:    \
    case OP_PUSH21:    \
    case OP_PUSH22:    \
    case OP_PUSH23:    \
    case OP_PUSH24:    \
    case OP_PUSH25:    \
    case OP_PUSH26:    \
    case OP_PUSH27:    \
    case OP_PUSH28:    \
    case OP_PUSH29:    \
    case OP_PUSH30:    \
    case OP_PUSH31:    \
    case OP_PUSH32:    \
    case OP_PUSH33:    \
    case OP_PUSH34:    \
    case OP_PUSH35:    \
    case OP_PUSH36:    \
    case OP_PUSH37:    \
    case OP_PUSH38:    \
    case OP_PUSH39:    \
    case OP_PUSH40:    \
    case OP_PUSH41:    \
    case OP_PUSH42:    \
    case OP_PUSH43:    \
    case OP_PUSH44:    \
    case OP_PUSH45:    \
    case OP_PUSH46:    \
    case OP_PUSH47:    \
    case OP_PUSH48:    \
    case OP_PUSH49:    \
    case OP_PUSH50:    \
    case OP_PUSH51:    \
    case OP_PUSH52:    \
    case OP_PUSH53:    \
    case OP_PUSH54:    \
    case OP_PUSH55:    \
    case OP_PUSH56:    \
    case OP_PUSH57:    \
    case OP_PUSH58:    \
    case OP_PUSH59:    \
    case OP_PUSH60:    \
    case OP_PUSH61:    \
    case OP_PUSH62:    \
    case OP_PUSH63:    \
    case OP_PUSH64
