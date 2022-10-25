/**
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include "srf/experimental/modules/segment_module_registry.hpp"
#include "srf/experimental/modules/segment_modules.hpp"
#include "srf/segment/builder.hpp"

#include <nlohmann/json.hpp>

namespace srf::modules {
struct ModelRegistryUtil
{
    /**
     * Helper function for registering a new module; automatically check that the type of the object is a segment
     * module, and build the constructor boiler plate.
     * @tparam ModuleTypeT Module type, must have modules::SegmentModule as a base class
     * @param name Name of the Module
     */
    template <typename ModuleTypeT>
    static void register_module(std::string name, std::string registry_namespace)
    {
        static_assert(std::is_base_of_v<modules::SegmentModule, ModuleTypeT>);

        ModuleRegistry::register_module(
            name, [](std::string module_name, nlohmann::json config) {
                return std::make_shared<ModuleTypeT>(std::move(module_name), std::move(config));
            }, registry_namespace);
    }
};
}  // namespace srf::modules