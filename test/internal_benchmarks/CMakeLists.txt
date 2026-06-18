# qrvmone: Fast Quantum Resistant Virtual Machine implementation
# Copyright 2019-2020 The evmone Authors.
# SPDX-License-Identifier: Apache-2.0

add_executable(
    qrvmone-bench-internal
    find_jumpdest_bench.cpp
    memory_allocation.cpp
)

target_link_libraries(qrvmone-bench-internal PRIVATE benchmark::benchmark)
