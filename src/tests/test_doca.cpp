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
  doca_context(std::string nic_addr, std::string gpu_addr)
  {
    std::cout << "ctor rte_eacl_context()" << std::endl;

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

    delete[] nic_addr_c;
    delete[] gpu_addr_c;

    if (eal_ret < 0) {
      throw std::runtime_error("EAL initialization failed, error=" + std::to_string(eal_ret));
    }

    doca_gpu* gpu;
    auto doca_ret = doca_gpu_create(gpu_addr_c, &gpu);

    if (doca_ret != DOCA_SUCCESS) {
      throw std::runtime_error("DOCA initialization failed, error=" + std::to_string(doca_ret));
    }

    
  }

  ~doca_context()
  {
    std::cout << "dtor rte_eacl_context()" << std::endl;

    auto ret = rte_eal_cleanup();

    if (ret < 0) {
      // log the fact that cleanup didn't work and move on.
        // throw std::runtime_error("rte eal cleanup failed, error=" + std::to_string(ret));
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
  std::make_shared<doca_context>("b6:00.0", "b5:00.0");
}
