// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2018 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

/// @file
/// QRVMC instance (class VM) and entry point of qrvmone is defined here.

#include "vm.hpp"
#include "advanced_execution.hpp"
#include "baseline.hpp"
#include <qrvmone/qrvmone.h>
#include <cassert>
#include <iostream>

namespace qrvmone
{
namespace
{
void destroy(qrvmc_vm* vm) noexcept
{
    assert(vm != nullptr);
    delete static_cast<VM*>(vm);
}

constexpr qrvmc_capabilities_flagset get_capabilities(qrvmc_vm* /*vm*/) noexcept
{
    return QRVMC_CAPABILITY_QRVM1;
}

qrvmc_set_option_result set_option(qrvmc_vm* c_vm, char const* c_name, char const* c_value) noexcept
{
    const auto name = (c_name != nullptr) ? std::string_view{c_name} : std::string_view{};
    const auto value = (c_value != nullptr) ? std::string_view{c_value} : std::string_view{};
    auto& vm = *static_cast<VM*>(c_vm);

    if (name == "advanced")
    {
        c_vm->execute = qrvmone::advanced::execute;
        return QRVMC_SET_OPTION_SUCCESS;
    }
    else if (name == "cgoto")
    {
#if QRVMONE_CGOTO_SUPPORTED
        if (value == "no")
        {
            vm.cgoto = false;
            return QRVMC_SET_OPTION_SUCCESS;
        }
        return QRVMC_SET_OPTION_INVALID_VALUE;
#else
        return QRVMC_SET_OPTION_INVALID_NAME;
#endif
    }
    else if (name == "trace")
    {
        vm.add_tracer(create_instruction_tracer(std::cerr));
        return QRVMC_SET_OPTION_SUCCESS;
    }
    else if (name == "histogram")
    {
        vm.add_tracer(create_histogram_tracer(std::cerr));
        return QRVMC_SET_OPTION_SUCCESS;
    }
    return QRVMC_SET_OPTION_INVALID_NAME;
}

}  // namespace


inline constexpr VM::VM() noexcept
  : qrvmc_vm{
        QRVMC_ABI_VERSION,
        "qrvmone",
        PROJECT_VERSION,
        qrvmone::destroy,
        qrvmone::baseline::execute,
        qrvmone::get_capabilities,
        qrvmone::set_option,
    }
{}

}  // namespace qrvmone

extern "C" {
QRVMC_EXPORT qrvmc_vm* qrvmc_create_qrvmone() noexcept
{
    return new qrvmone::VM{};
}
}
