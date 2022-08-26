# SPDX-FileCopyrightText: Copyright (c) 2020-2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

list(APPEND CMAKE_MESSAGE_CONTEXT "dep")

# Initialize rapids CPM with package overrides
rapids_cpm_init(OVERRIDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps/rapids_cpm_package_overrides.json")

# Print CMake settings when verbose output is enabled
message(VERBOSE "PROJECT_NAME: " ${PROJECT_NAME})
message(VERBOSE "CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM}")
message(VERBOSE "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
message(VERBOSE "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
message(VERBOSE "CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
message(VERBOSE "CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})
message(VERBOSE "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
message(VERBOSE "CMAKE_CUDA_COMPILER: " ${CMAKE_CUDA_COMPILER})
message(VERBOSE "CMAKE_CUDA_COMPILER_ID: " ${CMAKE_CUDA_COMPILER_ID})
message(VERBOSE "CMAKE_CUDA_COMPILER_VERSION: " ${CMAKE_CUDA_COMPILER_VERSION})
message(VERBOSE "CMAKE_CUDA_FLAGS: " ${CMAKE_CUDA_FLAGS})
message(VERBOSE "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(VERBOSE "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
message(VERBOSE "CMAKE_CURRENT_LIST_DIR: " ${CMAKE_CURRENT_LIST_DIR})
message(VERBOSE "CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS})
message(VERBOSE "CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})
message(VERBOSE "CMAKE_INSTALL_FULL_INCLUDEDIR: " ${CMAKE_INSTALL_FULL_INCLUDEDIR})
message(VERBOSE "CMAKE_INSTALL_FULL_LIBDIR: " ${CMAKE_INSTALL_FULL_LIBDIR})
message(VERBOSE "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
message(VERBOSE "CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH})
message(VERBOSE "CMAKE_FIND_ROOT_PATH: " ${CMAKE_FIND_ROOT_PATH})
message(VERBOSE "CMAKE_LIBRARY_ARCHITECTURE: " ${CMAKE_LIBRARY_ARCHITECTURE})
message(VERBOSE "FIND_LIBRARY_USE_LIB64_PATHS: " ${FIND_LIBRARY_USE_LIB64_PATHS})
message(VERBOSE "CMAKE_SYSROOT: " ${CMAKE_SYSROOT})
message(VERBOSE "CMAKE_STAGING_PREFIX: " ${CMAKE_STAGING_PREFIX})
message(VERBOSE "CMAKE_FIND_ROOT_PATH_MODE_INCLUDE: " ${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE})


find_package(Protobuf REQUIRED)

# Start with CUDA. Need to add it to our export set
rapids_find_package(CUDAToolkit
  REQUIRED
  BUILD_EXPORT_SET ${PROJECT_NAME}-core-exports
  INSTALL_EXPORT_SET ${PROJECT_NAME}-core-exports
)

# nvidia cub
# ==========
find_path(CUB_INCLUDE_DIRS "cub/cub.cuh"
  HINTS ${CUDAToolkit_INCLUDE_DIRS} ${Thrust_SOURCE_DIR} ${CUB_DIR}
  REQUIRE
)

# FlatBuffers
# ===========
#rapids_find_package(Flatbuffers REQUIRED
#  GLOBAL_TARGETS Flatbuffers
#  BUILD_EXPORT_SET ${PROJECT_NAME}-core-exports
#  INSTALL_EXPORT_SET ${PROJECT_NAME}-core-exports
#  FIND_ARGS
#  CONFIG
#)
include(deps/Configure_abseil)
include(deps/Configure_boost)
include(deps/Configure_ucx)
include(deps/Configure_hwloc)
include(deps/Configure_RMM)
include(deps/Configure_gflags)
include(deps/Configure_glog)
include(deps/Configure_gRPC)
include(deps/Configure_rxcpp)
include(deps/Configure_nlohmann_json)
include(deps/Configure_prometheus)
include(deps/Configure_libcudacxx)

if(SRF_BUILD_BENCHMARKS)
  # google benchmark
  # ================
  rapids_find_package(benchmark REQUIRED
    GLOBAL_TARGETS benchmark::benchmark
    BUILD_EXPORT_SET ${PROJECT_NAME}-core-exports
    # No install set
    FIND_ARGS
    CONFIG
    )
endif()

if(SRF_BUILD_TESTS)
  # google test
  # ===========
  rapids_find_package(GTest REQUIRED
    GLOBAL_TARGETS GTest::gtest GTest::gmock GTest::gtest_main GTest::gmock_main
    BUILD_EXPORT_SET ${PROJECT_NAME}-core-exports
    # No install set
    FIND_ARGS
    CONFIG
  )
endif()

list(POP_BACK CMAKE_MESSAGE_CONTEXT)
