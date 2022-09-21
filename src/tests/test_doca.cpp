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

#include "internal/doca/doca_context.hpp"
#include "internal/resources/manager.hpp"
#include "internal/system/system.hpp"

#include <srf/core/executor.hpp>
#include <srf/options/options.hpp>
#include <srf/segment/builder.hpp>
#include <srf/segment/definition.hpp>
#include <srf/pipeline/pipeline.hpp>

// #include <rte_eal.h>
// #include <doca_version.h>
// #include <doca_argp.h>
// #include <doca_gpu.h>
// #include <doca_flow.h>


#include <glog/logging.h>
#include <gtest/gtest.h>
#include <stdexcept>

using namespace srf;
using namespace srf::internal;

// static std::shared_ptr<system::System> make_system(std::function<void(Options&)> updater = nullptr)
// {
//     auto options = std::make_shared<Options>();
//     if (updater)
//     {
//         options->placement().resources_strategy(PlacementResources::Dedicated);
//         updater(*options);
//     }

//     return system::make_system(std::move(options));
// }

// static auto make_resources(std::function<void(Options&)> updater = nullptr)
// {
//     return std::make_unique<internal::resources::Manager>(internal::system::SystemProvider(make_system(updater)));
// }

void execute_pipeline(std::unique_ptr<srf::pipeline::Pipeline> pipeline)
{
    auto options = std::make_unique<Options>();
    options->topology().user_cpuset("0");
    Executor exec(std::move(options));
    exec.register_pipeline(std::move(pipeline));
    exec.start();
    exec.join();
}

// make -j 4 && sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH ./src/tests/test_srf_private.x --gtest_filter=TestDOCA.*

std::shared_ptr<srf::internal::doca::doca_context> _context;
std::mutex g_i_mutex;

class TestDOCA : public ::testing::Test
{
protected:
  void SetUp() override {
    const std::lock_guard<std::mutex> lock(g_i_mutex);
    if (_context == nullptr) {
      _context = std::make_shared<srf::internal::doca::doca_context>("b5:00.0", "b6:00.0"); 
    }
  }
};

TEST_F(TestDOCA, SetupAndTeardown)
{
    auto init = [](srf::segment::Builder& segment) {
        auto src = segment.make_source<std::string>("src", [&](rxcpp::subscriber<std::string>& s) {
            // loop packet reader, and forward data
            LOG(INFO) << "hello world" << std::endl;
            s.on_completed();
        });
    };

    auto segdef = srf::segment::Definition::create("segment_test", init);
    auto pipeline = srf::pipeline::make_pipeline();
    pipeline->register_segment(segdef);
    execute_pipeline(std::move(pipeline));
}
