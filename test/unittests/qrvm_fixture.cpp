// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2019-2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "qrvm_fixture.hpp"
#include <qrvmone/qrvmone.h>

namespace qrvmone::test
{
namespace
{
qrvmc::VM advanced_vm{qrvmc_create_qrvmone(), {{"advanced", ""}}};
qrvmc::VM baseline_vm{qrvmc_create_qrvmone()};
qrvmc::VM bnocgoto_vm{qrvmc_create_qrvmone(), {{"cgoto", "no"}}};

const char* print_vm_name(const testing::TestParamInfo<qrvmc::VM*>& info) noexcept
{
    if (info.param == &advanced_vm)
        return "advanced";
    if (info.param == &baseline_vm)
        return "baseline";
    if (info.param == &bnocgoto_vm)
        return "bnocgoto";
    return "unknown";
}
}  // namespace

INSTANTIATE_TEST_SUITE_P(
    qrvmone, qrvm, testing::Values(&advanced_vm, &baseline_vm, &bnocgoto_vm), print_vm_name);

bool qrvm::is_advanced() noexcept
{
    return GetParam() == &advanced_vm;
}
}  // namespace qrvmone::test
