/**
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "internal/resources/manager.hpp"
#include "internal/system/system.hpp"

#include "srf/options/options.hpp"

#include <rte_eal.h>
#include <doca_version.h>
#include <doca_argp.h>
#include <doca_gpu.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <stdexcept>

using namespace srf;
using namespace srf::internal;

static std::shared_ptr<system::System> make_system(std::function<void(Options&)> updater = nullptr)
{
    auto options = std::make_shared<Options>();
    if (updater)
    {
        options->placement().resources_strategy(PlacementResources::Dedicated);
        updater(*options);
    }

    return system::make_system(std::move(options));
}

static auto make_resources(std::function<void(Options&)> updater = nullptr)
{
    return std::make_unique<internal::resources::Manager>(internal::system::SystemProvider(make_system(updater)));
}

struct doca_context
{
private:
  doca_gpu* _gpu;

public:
  doca_context(std::string nic_addr, std::string gpu_addr)
  {
    char* nic_addr_c = new char[nic_addr.size() + 1];
    char* gpu_addr_c = new char[gpu_addr.size() + 1];

    nic_addr_c[nic_addr.size()] = '\0';
    gpu_addr_c[gpu_addr.size()] = '\0';

    std::copy(nic_addr.begin(), nic_addr.end(), nic_addr_c);
    std::copy(gpu_addr.begin(), gpu_addr.end(), gpu_addr_c);

    auto argv = std::vector<char*>();

    char a_flag[] = "-a";
    argv.push_back(a_flag);
    argv.push_back(nic_addr_c);
    argv.push_back(a_flag);
    argv.push_back(gpu_addr_c);

    char l_flag[] = "-l";
    char l_arg[] = "0,1,2,3,4";
    argv.push_back(l_flag);
    argv.push_back(l_arg);

    auto eal_ret = rte_eal_init(argv.size(), argv.data());
    if (eal_ret < 0) {
      throw std::runtime_error("EAL initialization failed, error=" + std::to_string(eal_ret));
    }

    auto doca_ret = doca_gpu_create(gpu_addr_c, &_gpu);
    if (doca_ret != DOCA_SUCCESS) {
      throw std::runtime_error("DOCA initialization failed, error=" + std::to_string(doca_ret));
    }

    auto gpu_attack_dpdk_ret = doca_gpu_to_dpdk(_gpu);
    if (gpu_attack_dpdk_ret != DOCA_SUCCESS) {
      throw std::runtime_error("DOCA to DPDK attach failed, error=" + std::to_string(gpu_attack_dpdk_ret));
    }

    delete[] nic_addr_c;
    delete[] gpu_addr_c;
  }

  ~doca_context()
  {
    auto eal_ret = rte_eal_cleanup();
    if (eal_ret < 0) {
      std::cout << "WARNING: EAL cleanup failed (" << eal_ret << ")" << std::endl;
    }

    auto doca_ret = doca_gpu_destroy(&_gpu);
    if (doca_ret != DOCA_SUCCESS) {
      std::cout << "WARNING: DOCA cleanup failed (" << doca_ret << ")" << std::endl;
    }
  }
};

// make -j 4 && sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH ./src/tests/test_srf_private.x --gtest_filter=TestDOCA.*

class TestDOCA : public ::testing::Test
{};

TEST_F(TestDOCA, Version)
{
    LOG(INFO) << "DOCA Version: " << doca_version();
}

TEST_F(TestDOCA, SetupAndTeardown)
{
  std::make_shared<doca_context>("b5:00.0", "b6:00.0");
}
