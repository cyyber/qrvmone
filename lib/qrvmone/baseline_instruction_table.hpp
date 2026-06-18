// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <qrvmc/qrvmc.h>
#include <array>

namespace qrvmone::baseline
{
using CostTable = std::array<int16_t, 256>;

const CostTable& get_baseline_cost_table(qrvmc_revision rev) noexcept;
}  // namespace qrvmone::baseline
