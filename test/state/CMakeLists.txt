# qrvmone: Fast Quantum Resistant Virtual Machine implementation
# Copyright 2022 The evmone Authors.
# SPDX-License-Identifier: Apache-2.0

add_library(qrvmone-state STATIC)
add_library(qrvmone::state ALIAS qrvmone-state)
target_link_libraries(qrvmone-state PUBLIC qrvmc::qrvmc_cpp PRIVATE qrvmone ethash::keccak)
target_include_directories(qrvmone-state PRIVATE ${qrvmone_private_include_dir})
target_sources(
    qrvmone-state PRIVATE
    account.hpp
    bloom_filter.hpp
    bloom_filter.cpp
    errors.hpp
    hash_utils.hpp
    hash_utils.cpp
    host.hpp
    host.cpp
    mpt.hpp
    mpt.cpp
    mpt_hash.hpp
    mpt_hash.cpp
    precompiles.hpp
    precompiles.cpp
    precompiles_cache.hpp
    precompiles_cache.cpp
    rlp.hpp
    state.hpp
    state.cpp
)
