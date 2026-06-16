// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#include "hash_utils.hpp"

std::ostream& operator<<(std::ostream& out, const qrvmone::address& a)
{
    return out << "0x" << hex(a);
}

std::ostream& operator<<(std::ostream& out, const qrvmone::bytes64& b)
{
    return out << "0x" << hex(b);
}
