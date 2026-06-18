# qrvmone: Fast Quantum Resistant Virtual Machine implementation
# Copyright 2022 The evmone Authors.
# SPDX-License-Identifier: Apache-2.0

include(CableBuildInfo)
cable_add_buildinfo_library(PROJECT_NAME qrvmone)

hunter_add_package(nlohmann_json)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(qrvmone-t8n)
target_link_libraries(qrvmone-t8n PRIVATE qrvmone::statetestutils nlohmann_json::nlohmann_json)
target_link_libraries(qrvmone-t8n PRIVATE qrvmc::qrvmc qrvmone qrvmone-buildinfo)
target_sources(qrvmone-t8n PRIVATE t8n.cpp)
