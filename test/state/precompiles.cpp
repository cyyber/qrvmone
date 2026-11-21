// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2022 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "precompiles.hpp"
#include "precompiles_cache.hpp"
#include <intx/intx.hpp>
#include <bit>
#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <unordered_map>

namespace qrvmone::state
{
using namespace qrvmc::literals;

namespace
{
constexpr auto GasCostMax = std::numeric_limits<int64_t>::max();

struct PrecompileAnalysis
{
    int64_t gas_cost;
    size_t max_output_size;
};

inline constexpr int64_t num_words(size_t size_in_bytes) noexcept
{
    return static_cast<int64_t>((size_in_bytes + 31) / 32);
}

template <int BaseCost, int WordCost>
inline constexpr int64_t cost_per_input_word(size_t input_size) noexcept
{
    return BaseCost + WordCost * num_words(input_size);
}

PrecompileAnalysis depositroot_analyze(bytes_view /*input*/, qrvmc_revision /*rev*/) noexcept
{
    return {19992, 32};
}

PrecompileAnalysis sha256_analyze(bytes_view input, qrvmc_revision /*rev*/) noexcept
{
    return {cost_per_input_word<60, 12>(input.size()), 32};
}

PrecompileAnalysis identity_analyze(bytes_view input, qrvmc_revision /*rev*/) noexcept
{
    return {cost_per_input_word<15, 3>(input.size()), input.size()};
}

PrecompileAnalysis ecadd_analyze(bytes_view /*input*/, qrvmc_revision /*rev*/) noexcept
{
    return {150, 64};
}

PrecompileAnalysis ecmul_analyze(bytes_view /*input*/, qrvmc_revision /*rev*/) noexcept
{
    return {6000, 64};
}

PrecompileAnalysis ecpairing_analyze(bytes_view input, qrvmc_revision /*rev*/) noexcept
{
    const auto base_cost = 45000;
    const auto element_cost = 34000;
    const auto num_elements = static_cast<int64_t>(input.size() / 192);
    return {base_cost + num_elements * element_cost, 32};
}

PrecompileAnalysis expmod_analyze(bytes_view input, qrvmc_revision /*rev*/) noexcept
{
    using namespace intx;

    // The gas cost of EIP-7883 and the length limit of EIP-7823, as go-qrl's bigModExp
    // has them (cyyber/go-qrl 6392e39). They are not revision dependent there.
    static constexpr size_t input_header_required_size = 3 * sizeof(uint256);
    static constexpr int64_t min_gas = 500;
    static constexpr size_t len_limit = 1024;

    uint8_t input_header[input_header_required_size]{};
    std::copy_n(input.data(), std::min(input.size(), input_header_required_size), input_header);

    const auto base_len256 = be::unsafe::load<uint256>(&input_header[0]);
    const auto exp_len256 = be::unsafe::load<uint256>(&input_header[32]);
    const auto mod_len256 = be::unsafe::load<uint256>(&input_header[64]);

    // EIP-7823: go-qrl fails the call if any of the lengths is over the limit,
    // and a failed precompile call takes all the gas it was given.
    if (base_len256 > len_limit || exp_len256 > len_limit || mod_len256 > len_limit)
        return {GasCostMax, 0};

    const auto base_len = static_cast<size_t>(base_len256);
    const auto exp_len = static_cast<size_t>(exp_len256);
    const auto mod_len = static_cast<size_t>(mod_len256);

    auto adjusted_len = [input](size_t offset, size_t len) {
        // The part of the first 32 bytes of the exponent which is in the input.
        // The input may end before it does or even before it starts: the rest is zeros.
        const auto head_len = std::min(len, size_t{32});
        const auto head_explicit_bytes = input.substr(std::min(offset, input.size()), head_len);
        const auto top_byte_index = head_explicit_bytes.find_first_not_of(uint8_t{0});
        const size_t exp_bit_width =
            (top_byte_index != bytes_view::npos) ?
                (head_len - top_byte_index - 1) * 8 +
                    static_cast<size_t>(std::bit_width(head_explicit_bytes[top_byte_index])) :
                0;

        return std::max(
            16 * (std::max(len, size_t{32}) - 32) + (std::max(exp_bit_width, size_t{1}) - 1),
            size_t{1});
    };

    static constexpr auto mult_complexity_eip7883 = [](size_t max_len) noexcept -> size_t {
        if (max_len <= 32)
            return 16;
        const auto w = (max_len + 7) / 8;
        return 2 * w * w;
    };

    // Within the length limit the cost is at most 2 * 128^2 * (16 * 992 + 255): it cannot overflow.
    const auto max_len = std::max(mod_len, base_len);
    const auto adjusted_exp_len = adjusted_len(sizeof(input_header) + base_len, exp_len);
    const auto gas = mult_complexity_eip7883(max_len) * adjusted_exp_len;
    return {std::max(min_gas, static_cast<int64_t>(gas)), mod_len};
}

ExecutionResult identity_execute(const uint8_t* input, size_t input_size, uint8_t* output,
    [[maybe_unused]] size_t output_size) noexcept
{
    assert(output_size == input_size);
    std::copy_n(input, input_size, output);
    return {QRVMC_SUCCESS, input_size};
}

struct PrecompileTraits
{
    decltype(identity_analyze)* analyze = nullptr;
    decltype(identity_execute)* execute = nullptr;
};

template <PrecompileId Id>
ExecutionResult dummy_execute(const uint8_t*, size_t, uint8_t*, size_t) noexcept
{
    std::cerr << "Precompile " << static_cast<int>(Id) << " not implemented!\n";
    return ExecutionResult{QRVMC_INTERNAL_ERROR, 0};
}

/// The traits of the precompiles indexed by the precompile id.
/// The ids no precompile is assigned to (0, 3, 9) are left with the null traits.
inline constexpr auto traits = []() noexcept {
    std::array<PrecompileTraits, NumPrecompiles> tbl{};
    const auto at = [&tbl](PrecompileId id) noexcept -> PrecompileTraits& {
        return tbl[stdx::to_underlying(id)];
    };
    at(PrecompileId::depositroot) = {depositroot_analyze, dummy_execute<PrecompileId::depositroot>};
    at(PrecompileId::sha256) = {sha256_analyze, dummy_execute<PrecompileId::sha256>};
    at(PrecompileId::identity) = {identity_analyze, identity_execute};
    at(PrecompileId::expmod) = {expmod_analyze, dummy_execute<PrecompileId::expmod>};
    at(PrecompileId::ecadd) = {ecadd_analyze, dummy_execute<PrecompileId::ecadd>};
    at(PrecompileId::ecmul) = {ecmul_analyze, dummy_execute<PrecompileId::ecmul>};
    at(PrecompileId::ecpairing) = {ecpairing_analyze, dummy_execute<PrecompileId::ecpairing>};
    return tbl;
}();
}  // namespace

std::optional<qrvmc::Result> call_precompile(qrvmc_revision rev, const qrvmc_message& msg) noexcept
{
    // Define compile-time constant,
    // TODO: workaround for Clang Analyzer bug https://github.com/llvm/llvm-project/issues/59493.
    static constexpr qrvmc::address address_boundary{NumPrecompiles};

    if (qrvmc::is_zero(msg.code_address) || msg.code_address >= address_boundary)
        return {};

    // 64-byte-address layout: precompile id is the last byte (bytes[63]),
    // mirroring the address(uint64_t) constructor that right-aligns the
    // integer at bytes[56..63].
    const auto id = msg.code_address.bytes[63];

    assert(id > 0);
    assert(msg.gas >= 0);

    const auto [analyze, execute] = traits[id];
    if (analyze == nullptr)
        return {};  // No precompile at this address.

    const bytes_view input{msg.input_data, msg.input_size};
    const auto [gas_cost, max_output_size] = analyze(input, rev);
    const auto gas_left = msg.gas - gas_cost;
    if (gas_left < 0)
        return qrvmc::Result{QRVMC_OUT_OF_GAS};

    static Cache cache;
    if (auto r = cache.find(static_cast<PrecompileId>(id), input, gas_left); r.has_value())
        return r;

    // The output size has no fixed bound: "identity" returns as much as it is given.
    const auto output_buf = std::make_unique<uint8_t[]>(max_output_size);

    const auto [status_code, output_size] =
        execute(msg.input_data, msg.input_size, output_buf.get(), max_output_size);
    assert(output_size <= max_output_size);

    qrvmc::Result result{
        status_code, status_code == QRVMC_SUCCESS ? gas_left : 0, 0, output_buf.get(), output_size};

    cache.insert(static_cast<PrecompileId>(id), input, result);

    return result;
}
}  // namespace qrvmone::state
