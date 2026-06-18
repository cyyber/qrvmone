// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2022 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <ethash/keccak.hpp>
#include <intx/intx.hpp>
#include <qrvmc/hex.hpp>
#include <qrvmc/qrvmc.hpp>
#include <cstring>

namespace qrvmone
{
using qrvmc::address;
using qrvmc::bytes;
using qrvmc::bytes64;
using qrvmc::bytes_view;
using namespace qrvmc::literals;

/// Default type for 256-bit hash.
///
/// Better than ethash::hash256 because has some additional handy constructors.
/// After the 512-bit VM migration qrvmc::bytes64 is 64 bytes wide; a 256-bit
/// Keccak hash lives in the low 32 bytes (bytes[32:64]), with the upper 32
/// bytes zeroed. That matches the `store_uint` convention used on the VM
/// stack/storage paths for 256-bit-sized values.
using hash256 = bytes64;

/// Computes Keccak hash out of input bytes (wrapper of ethash::keccak256).
inline hash256 keccak256(bytes_view data) noexcept
{
    const auto eh = ethash::keccak256(data.data(), data.size());
    hash256 h{};  // zero-initialise both halves of the 64-byte container
    static_assert(sizeof(eh.bytes) == 32, "ethash::hash256 is always 32 bytes");
    std::memcpy(h.bytes + (sizeof(h.bytes) - sizeof(eh.bytes)), eh.bytes, sizeof(eh.bytes));
    return h;
}

/// Pack an intx::uint256 into the 64-byte qrvmc wire representation.
/// The integer's big-endian bytes land in the low 32 bytes
/// (bytes[32:64]); the upper 32 bytes are zero. Matches the
/// `store_uint` convention used by qrvmone's core for 256-bit values
/// transported through a qrvmc_bytes64 field (balance, msg.value, ...).
inline qrvmc::bytes64 to_be256(const intx::uint256& x) noexcept
{
    qrvmc::bytes64 r{};
    intx::be::store(*reinterpret_cast<uint8_t(*)[32]>(&r.bytes[32]), x);
    return r;
}

/// Reverse of to_be256: read the low 32 bytes of a qrvmc_bytes64 as an
/// intx::uint256, ignoring the zero-padded upper half.
inline intx::uint256 from_be256(const qrvmc::bytes64& r) noexcept
{
    return intx::be::load<intx::uint256>(
        *reinterpret_cast<const uint8_t(*)[32]>(&r.bytes[32]));
}
}  // namespace qrvmone

std::ostream& operator<<(std::ostream& out, const qrvmone::address& a);
std::ostream& operator<<(std::ostream& out, const qrvmone::bytes64& b);
