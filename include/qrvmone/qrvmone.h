// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2018-2019 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#ifndef QRVMONE_H
#define QRVMONE_H

#include <qrvmc/utils.h>
#include <qrvmc/qrvmc.h>

#if __cplusplus
extern "C" {
#endif

QRVMC_EXPORT struct qrvmc_vm* qrvmc_create_qrvmone(void) QRVMC_NOEXCEPT;

#if __cplusplus
}
#endif

#endif  // QRVMONE_H
