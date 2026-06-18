// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2018 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <qrvmc/utils.h>
#include <qrvmc/qrvmc.h>

namespace qrvmone::advanced
{
struct AdvancedExecutionState;
struct AdvancedCodeAnalysis;

/// Execute the already analyzed code using the provided execution state.
QRVMC_EXPORT qrvmc_result execute(
    AdvancedExecutionState& state, const AdvancedCodeAnalysis& analysis) noexcept;

/// QRVMC-compatible execute() function.
qrvmc_result execute(qrvmc_vm* vm, const qrvmc_host_interface* host, qrvmc_host_context* ctx,
    qrvmc_revision rev, const qrvmc_message* msg, const uint8_t* code, size_t code_size) noexcept;
}  // namespace qrvmone::advanced
