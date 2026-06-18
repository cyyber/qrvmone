// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2021 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "tracing.hpp"
#include <qrvmc/qrvmc.h>

#if defined(_MSC_VER) && !defined(__clang__)
#define QRVMONE_CGOTO_SUPPORTED 0
#else
#define QRVMONE_CGOTO_SUPPORTED 1
#endif

namespace qrvmone
{
/// The qrvmone QRVMC instance.
class VM : public qrvmc_vm
{
public:
    bool cgoto = QRVMONE_CGOTO_SUPPORTED;

private:
    std::unique_ptr<Tracer> m_first_tracer;

public:
    inline constexpr VM() noexcept;

    void add_tracer(std::unique_ptr<Tracer> tracer) noexcept
    {
        // Find the first empty unique_ptr and assign the new tracer to it.
        auto* end = &m_first_tracer;
        while (*end)
            end = &(*end)->m_next_tracer;
        *end = std::move(tracer);
    }

    [[nodiscard]] Tracer* get_tracer() const noexcept { return m_first_tracer.get(); }
};
}  // namespace qrvmone
