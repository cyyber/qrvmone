// qrvmone: Fast Quantum Resistant Virtual Machine implementation
// Copyright 2019-2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <qrvmc/qrvmc.hpp>
#include <qrvmone/vm.hpp>
#include <qrvmone/qrvmone.h>

TEST(qrvmone, info)
{
    auto vm = qrvmc::VM{qrvmc_create_qrvmone()};
    EXPECT_STREQ(vm.name(), "qrvmone");
    EXPECT_STREQ(vm.version(), PROJECT_VERSION);
    EXPECT_TRUE(vm.is_abi_compatible());
}

TEST(qrvmone, capabilities)
{
    auto vm = qrvmc_create_qrvmone();
    EXPECT_EQ(vm->get_capabilities(vm), qrvmc_capabilities_flagset{QRVMC_CAPABILITY_QRVM1});
    vm->destroy(vm);
}

TEST(qrvmone, set_option_invalid)
{
    auto vm = qrvmc_create_qrvmone();
    ASSERT_NE(vm->set_option, nullptr);
    EXPECT_EQ(vm->set_option(vm, "", ""), QRVMC_SET_OPTION_INVALID_NAME);
    EXPECT_EQ(vm->set_option(vm, "o", ""), QRVMC_SET_OPTION_INVALID_NAME);
    EXPECT_EQ(vm->set_option(vm, "0", ""), QRVMC_SET_OPTION_INVALID_NAME);
    vm->destroy(vm);
}

TEST(qrvmone, set_option_advanced)
{
    auto vm = qrvmc::VM{qrvmc_create_qrvmone()};
    EXPECT_EQ(vm.set_option("advanced", ""), QRVMC_SET_OPTION_SUCCESS);

    // This will also enable Advanced.
    EXPECT_EQ(vm.set_option("advanced", "no"), QRVMC_SET_OPTION_SUCCESS);
}

TEST(qrvmone, set_option_cgoto)
{
    qrvmc::VM vm{qrvmc_create_qrvmone()};

#if QRVMONE_CGOTO_SUPPORTED
    EXPECT_EQ(vm.set_option("cgoto", ""), QRVMC_SET_OPTION_INVALID_VALUE);
    EXPECT_EQ(vm.set_option("cgoto", "yes"), QRVMC_SET_OPTION_INVALID_VALUE);
    EXPECT_EQ(vm.set_option("cgoto", "no"), QRVMC_SET_OPTION_SUCCESS);
#else
    EXPECT_EQ(vm.set_option("cgoto", "no"), QRVMC_SET_OPTION_INVALID_NAME);
#endif
}
