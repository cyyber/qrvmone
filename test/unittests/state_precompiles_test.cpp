// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <intx/intx.hpp>
#include <test/state/precompiles.hpp>
#include <test/utils/utils.hpp>
#include <iostream>
#include <sstream>
#include <string_view>

using namespace qrvmc;
using namespace qrvmone;

namespace
{
constexpr uint8_t identity_id = 0x04;
constexpr uint8_t expmod_id = 0x05;

/// More gas than any accepted expmod input costs.
constexpr int64_t plenty_of_gas = 30'000'000'000;

class state_precompiles : public testing::Test
{
    /// The precompiles without an implementation report themselves to std::cerr when executed.
    /// This is expected here (expmod is one of them), so keep it out of the test output.
    std::ostringstream m_cerr_sink;
    std::streambuf* m_cerr_buf = nullptr;

protected:
    void SetUp() override { m_cerr_buf = std::cerr.rdbuf(m_cerr_sink.rdbuf()); }
    void TearDown() override { std::cerr.rdbuf(m_cerr_buf); }

    static std::optional<Result> call(uint8_t id, bytes_view input, int64_t gas)
    {
        qrvmc_message msg{};
        msg.code_address.bytes[sizeof(msg.code_address.bytes) - 1] = id;
        msg.gas = gas;
        msg.input_data = input.data();
        msg.input_size = input.size();
        return state::call_precompile(QRVMC_ZOND, msg);
    }

    /// Checks that the precompile charges exactly the given amount of gas:
    /// it runs out of gas when given one less and does not when given precisely that much.
    /// This works also for the precompiles without an implementation.
    static void expect_gas_cost(uint8_t id, bytes_view input, int64_t gas_cost)
    {
        const auto too_little = call(id, input, gas_cost - 1);
        ASSERT_TRUE(too_little.has_value());
        EXPECT_EQ(too_little->status_code, QRVMC_OUT_OF_GAS);

        const auto enough = call(id, input, gas_cost);
        ASSERT_TRUE(enough.has_value());
        EXPECT_NE(enough->status_code, QRVMC_OUT_OF_GAS);
    }

    /// Checks that the call fails and no gas is left whatever amount is given.
    static void expect_all_gas_burned(uint8_t id, bytes_view input)
    {
        for (const auto gas : {int64_t{0}, int64_t{1'000'000}, plenty_of_gas})
        {
            const auto r = call(id, input, gas);
            ASSERT_TRUE(r.has_value());
            EXPECT_EQ(r->status_code, QRVMC_OUT_OF_GAS);
            EXPECT_EQ(r->gas_left, 0);
        }
    }
};

bytes word(const intx::uint256& v)
{
    uint8_t buf[32];
    intx::be::store(buf, v);
    return bytes{buf, sizeof(buf)};
}

/// The expmod input header: the lengths of the base, the exponent and the modulus.
bytes expmod_header(
    const intx::uint256& base_len, const intx::uint256& exp_len, const intx::uint256& mod_len)
{
    return word(base_len) + word(exp_len) + word(mod_len);
}

/// The complete expmod input with the given lengths and the leading bytes of the exponent.
/// The gas cost depends on nothing else, so the rest of the operands is zeros.
bytes expmod_input(size_t base_len, size_t exp_len, size_t mod_len, bytes_view exp_head = {})
{
    auto exponent = bytes{exp_head};
    exponent.resize(exp_len);
    return expmod_header(base_len, exp_len, mod_len) + bytes(base_len, 0) + exponent +
           bytes(mod_len, 0);
}
}  // namespace

// The expected gas costs are the ones required by go-qrl's bigModExp (cyyber/go-qrl 6392e39),
// which prices the precompile by EIP-7883. These are cases of its modexp.json test vectors,
// reduced to what the gas cost depends on: the lengths and the leading bytes of the exponent.
TEST_F(state_precompiles, expmod_gas_cost)
{
    struct TestCase
    {
        std::string_view name;  ///< The name of the case in go-qrl's modexp.json.
        size_t base_len;
        size_t exp_len;
        size_t mod_len;
        bytes exp_head;
        int64_t gas_cost;
    };

    const auto ff32 = bytes(32, 0xff);
    const auto eip_example_head =
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2e"_hex;
    const auto marius_1_head =
        "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffe000007d"_hex;
    const auto guido_1_head =
        "1cffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"_hex;
    const auto guido_2_head =
        "2b212121ffff1f1f1f1f1f1f1f1f1f1f1f1f1f1f1f1f1f1f1f1f1f1f00feffff"_hex;

    const TestCase test_cases[] = {
        {"eip_example2", 0, 32, 32, eip_example_head, 4080},
        {"nagydani-1-square", 64, 1, 64, "02"_hex, 500},
        {"nagydani-1-pow0x10001", 64, 3, 64, "010001"_hex, 2048},
        {"nagydani-2-square", 128, 1, 128, "02"_hex, 512},
        {"nagydani-2-pow0x10001", 128, 3, 128, "010001"_hex, 8192},
        {"nagydani-5-qube", 1024, 1, 1024, "03"_hex, 32768},
        {"nagydani-5-pow0x10001", 1024, 3, 1024, "010001"_hex, 524288},
        {"marius-1-even", 3, 193, 12, marius_1_head, 45296},
        {"guido-1-even", 16, 216, 16, guido_1_head, 51136},
        {"guido-2-even", 16, 216, 16, guido_2_head, 51152},
        {"guido-3-even", 487, 1, 715, "04"_hex, 32400},
        {"guido-4-even", 1, 385, 8, ff32, 94448},
        {"marcin-1-base-heavy", 192, 1, 192, "03"_hex, 1152},
        {"marcin-1-exp-heavy", 8, 81, 8, ff32, 16624},
        {"marcin-1-balanced", 40, 4, 40, "01ffffff"_hex, 1200},
        {"marcin-2-base-heavy", 408, 1, 408, "03"_hex, 5202},
        {"marcin-2-balanced", 56, 8, 56, "2bffffffffffffff"_hex, 5978},
        {"mod-32-exp-32", 32, 4, 32, "ffffffff"_hex, 500},
        {"mod-32-exp-36", 32, 5, 32, "0dffffffff"_hex, 560},
        {"mod-32-exp-40", 32, 5, 32, "ffffffffff"_hex, 624},
        {"mod-32-exp-64", 32, 8, 32, "ffffffffffffffff"_hex, 1008},
        {"mod-32-exp-65", 32, 9, 32, "01ffffffffffffffff"_hex, 1024},
        {"mod-264-exp-2", 264, 1, 264, "03"_hex, 2178},
        {"mod_vul_pawel_3_exp_8", 23, 8, 23, "ffffffffffffffff"_hex, 1008},
    };

    for (const auto& t : test_cases)
    {
        SCOPED_TRACE(t.name);
        expect_gas_cost(
            expmod_id, expmod_input(t.base_len, t.exp_len, t.mod_len, t.exp_head), t.gas_cost);
    }
}

// The expected gas costs have been produced by go-qrl's bigModExp (cyyber/go-qrl 6392e39).
TEST_F(state_precompiles, expmod_gas_cost_boundaries)
{
    const auto ff32 = bytes(32, 0xff);
    const auto ff5 = bytes(5, 0xff);

    // The minimum cost.
    expect_gas_cost(expmod_id, {}, 500);

    // Empty base and modulus are not free: the minimum multiplication complexity is 16.
    expect_gas_cost(expmod_id, expmod_input(0, 32, 0, ff32), 4080);
    expect_gas_cost(expmod_id, expmod_header(0, 1024, 0), 253952);

    // The multiplication complexity is flat up to 32 bytes and quadratic (doubled) above.
    expect_gas_cost(expmod_id, expmod_input(32, 5, 32, ff5), 624);
    expect_gas_cost(expmod_id, expmod_input(33, 5, 33, ff5), 1950);

    // Every exponent byte above 32 adds 16 iterations.
    expect_gas_cost(expmod_id, expmod_input(1, 32, 1, ff32), 4080);
    expect_gas_cost(expmod_id, expmod_input(1, 33, 1, ff32), 4336);

    // The input ending inside the exponent: the missing bytes are zeros,
    // so the single byte present is the top one of the 32-byte exponent.
    expect_gas_cost(expmod_id, expmod_header(1, 32, 1) + "00"_hex + "01"_hex, 3968);

    // The input ending inside the header: the missing lengths are zeros.
    expect_gas_cost(expmod_id, word(64), 500);

    // All lengths at the EIP-7823 limit, including the most expensive input possible.
    expect_gas_cost(expmod_id, expmod_header(1024, 1024, 1024), 520093696);
    expect_gas_cost(expmod_id, expmod_input(1024, 1024, 1024, ff32), 528449536);
}

// EIP-7823: go-qrl fails such a call (errModExpInputTooLong) which burns all its gas.
TEST_F(state_precompiles, expmod_length_limit)
{
    const auto overflowing_uint64 = intx::uint256{1} << 64;
    const auto max = ~intx::uint256{0};

    expect_all_gas_burned(expmod_id, expmod_header(1025, 0, 0));
    expect_all_gas_burned(expmod_id, expmod_header(0, 1025, 0));
    expect_all_gas_burned(expmod_id, expmod_header(0, 0, 1025));
    expect_all_gas_burned(expmod_id, expmod_header(overflowing_uint64, 0, 0));
    expect_all_gas_burned(expmod_id, expmod_header(0, overflowing_uint64, 0));
    expect_all_gas_burned(expmod_id, expmod_header(0, 0, overflowing_uint64));
    expect_all_gas_burned(expmod_id, expmod_header(max, max, max));
}

TEST_F(state_precompiles, identity)
{
    // The input is longer than 256 bytes on purpose: the output must not be limited by that.
    auto input = bytes(300, 0);
    for (size_t i = 0; i < input.size(); ++i)
        input[i] = static_cast<uint8_t>(i);

    const auto gas_cost = 15 + 3 * 10;  // 300 bytes is 10 words.
    expect_gas_cost(identity_id, input, gas_cost);

    const auto r = call(identity_id, input, gas_cost + 7);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->status_code, QRVMC_SUCCESS);
    EXPECT_EQ(r->gas_left, 7);
    EXPECT_EQ(bytes_view(r->output_data, r->output_size), input);

    const auto empty = call(identity_id, {}, 15);
    ASSERT_TRUE(empty.has_value());
    EXPECT_EQ(empty->status_code, QRVMC_SUCCESS);
    EXPECT_EQ(empty->gas_left, 0);
    EXPECT_EQ(empty->output_size, 0);
}

TEST_F(state_precompiles, not_a_precompile)
{
    EXPECT_FALSE(call(0x00, {}, plenty_of_gas).has_value());

    // Within the range of the precompile ids, but not assigned.
    EXPECT_FALSE(call(0x09, {}, plenty_of_gas).has_value());

    EXPECT_FALSE(call(0x0a, {}, plenty_of_gas).has_value());
    EXPECT_FALSE(call(0xff, {}, plenty_of_gas).has_value());
}
