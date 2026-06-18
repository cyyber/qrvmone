// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2019 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "test/utils/utils.hpp"
#include <benchmark/benchmark.h>
#include <qrvmc/mocked_host.hpp>
#include <qrvmc/qrvmc.hpp>
#include <qrvmone/advanced_analysis.hpp>
#include <qrvmone/advanced_execution.hpp>
#include <qrvmone/baseline.hpp>
#include <qrvmone/vm.hpp>

namespace qrvmone::test
{
extern std::map<std::string_view, qrvmc::VM> registered_vms;

constexpr auto default_revision = QRVMC_ZOND;
constexpr auto default_gas_limit = std::numeric_limits<int64_t>::max();


template <typename ExecutionStateT, typename AnalysisT>
using ExecuteFn = qrvmc::Result(qrvmc::VM& vm, ExecutionStateT& exec_state, const AnalysisT&,
    const qrvmc_message&, qrvmc_revision, qrvmc::Host&, bytes_view);

template <typename AnalysisT>
using AnalyseFn = AnalysisT(qrvmc_revision, bytes_view);


struct FakeExecutionState
{};

struct FakeCodeAnalysis
{};

inline advanced::AdvancedCodeAnalysis advanced_analyse(qrvmc_revision rev, bytes_view code)
{
    return advanced::analyze(rev, code);
}

inline baseline::CodeAnalysis baseline_analyse(qrvmc_revision rev, bytes_view code)
{
    return baseline::analyze(rev, code);
}

inline FakeCodeAnalysis qrvmc_analyse(qrvmc_revision /*rev*/, bytes_view /*code*/)
{
    return {};
}


inline qrvmc::Result advanced_execute(qrvmc::VM& /*vm*/, advanced::AdvancedExecutionState& exec_state,
    const advanced::AdvancedCodeAnalysis& analysis, const qrvmc_message& msg, qrvmc_revision rev,
    qrvmc::Host& host, bytes_view code)
{
    exec_state.reset(msg, rev, host.get_interface(), host.to_context(), code);
    return qrvmc::Result{execute(exec_state, analysis)};
}

inline qrvmc::Result baseline_execute(qrvmc::VM& c_vm, ExecutionState& exec_state,
    const baseline::CodeAnalysis& analysis, const qrvmc_message& msg, qrvmc_revision rev,
    qrvmc::Host& host, bytes_view code)
{
    const auto& vm = *static_cast<qrvmone::VM*>(c_vm.get_raw_pointer());
    exec_state.reset(msg, rev, host.get_interface(), host.to_context(), code);
    return qrvmc::Result{baseline::execute(vm, msg.gas, exec_state, analysis)};
}

inline qrvmc::Result qrvmc_execute(qrvmc::VM& vm, FakeExecutionState& /*exec_state*/,
    const FakeCodeAnalysis& /*analysis*/, const qrvmc_message& msg, qrvmc_revision rev,
    qrvmc::Host& host, bytes_view code) noexcept
{
    return vm.execute(host, rev, msg, code.data(), code.size());
}


template <typename AnalysisT, AnalyseFn<AnalysisT> analyse_fn>
inline void bench_analyse(benchmark::State& state, qrvmc_revision rev, bytes_view code) noexcept
{
    auto bytes_analysed = uint64_t{0};
    for (auto _ : state)
    {
        auto r = analyse_fn(rev, code);
        benchmark::DoNotOptimize(&r);
        bytes_analysed += code.size();
    }

    using benchmark::Counter;
    state.counters["size"] = Counter(static_cast<double>(code.size()));
    state.counters["rate"] = Counter(static_cast<double>(bytes_analysed), Counter::kIsRate);
}


template <typename ExecutionStateT, typename AnalysisT,
    ExecuteFn<ExecutionStateT, AnalysisT> execute_fn, AnalyseFn<AnalysisT> analyse_fn>
inline void bench_execute(benchmark::State& state, qrvmc::VM& vm, bytes_view code, bytes_view input,
    bytes_view expected_output) noexcept
{
    constexpr auto rev = default_revision;
    constexpr auto gas_limit = default_gas_limit;

    const auto analysis = analyse_fn(rev, code);
    qrvmc::MockedHost host;
    ExecutionStateT exec_state;
    qrvmc_message msg{};
    msg.kind = QRVMC_CALL;
    msg.gas = gas_limit;
    msg.input_data = input.data();
    msg.input_size = input.size();


    {  // Test run.
        const auto r = execute_fn(vm, exec_state, analysis, msg, rev, host, code);
        if (r.status_code != QRVMC_SUCCESS)
        {
            state.SkipWithError(("failure: " + std::to_string(r.status_code)).c_str());
            return;
        }

        if (!expected_output.empty())
        {
            const auto output = bytes_view{r.output_data, r.output_size};
            if (output != expected_output)
            {
                state.SkipWithError(
                    ("got: " + hex(output) + "  expected: " + hex(expected_output)).c_str());
                return;
            }
        }
    }

    auto total_gas_used = int64_t{0};
    auto iteration_gas_used = int64_t{0};
    for (auto _ : state)
    {
        const auto r = execute_fn(vm, exec_state, analysis, msg, rev, host, code);
        iteration_gas_used = gas_limit - r.gas_left;
        total_gas_used += iteration_gas_used;
    }

    using benchmark::Counter;
    state.counters["gas_used"] = Counter(static_cast<double>(iteration_gas_used));
    state.counters["gas_rate"] = Counter(static_cast<double>(total_gas_used), Counter::kIsRate);
}


constexpr auto bench_advanced_execute = bench_execute<advanced::AdvancedExecutionState,
    advanced::AdvancedCodeAnalysis, advanced_execute, advanced_analyse>;

constexpr auto bench_baseline_execute =
    bench_execute<ExecutionState, baseline::CodeAnalysis, baseline_execute, baseline_analyse>;

inline void bench_qrvmc_execute(benchmark::State& state, qrvmc::VM& vm, bytes_view code,
    bytes_view input = {}, bytes_view expected_output = {})
{
    bench_execute<FakeExecutionState, FakeCodeAnalysis, qrvmc_execute, qrvmc_analyse>(
        state, vm, code, input, expected_output);
}

}  // namespace qrvmone::test
