#=============================================================================
# SPDX-FileCopyrightText: Copyright (c) 2020-2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=============================================================================

function(find_and_configure_gRPC version)

  list(APPEND CMAKE_MESSAGE_CONTEXT "gRPC")

  rapids_cpm_find(gRPC ${version}
    GLOBAL_TARGETS
      gpr gRPC::gpr
      grpc gRPC::grpc
      grpc_cpp_plugin gRPC::grpc_cpp_plugin
      grpc++ gRPC::grpc++
    BUILD_EXPORT_SET
      ${PROJECT_NAME}-core-exports
    INSTALL_EXPORT_SET
      ${PROJECT_NAME}-core-exports
    CPM_ARGS
      GIT_REPOSITORY            https://github.com/grpc/grpc.git
      GIT_TAG                   v${version}
      GIT_SHALLOW               TRUE
      GIT_SUBMODULES_RECURSE    ON
      OPTIONS                   "BUILD_TESTS OFF"
                                "BUILD_BENCHMARKS OFF"
                                "CUDA_STATIC_RUNTIME ON"
                                "DISABLE_DEPRECATION_WARNING ${DISABLE_DEPRECATION_WARNINGS}"
                                "gRPC_BUILD_CODEGEN ON"
                                "gRPC_BUILD_GRPC_CPP_PLUGIN ON"
                                "gRPC_INSTALL ON"
                                "gRPC_ABSL_PROVIDER package"
  )

  if (gRPC_ADDED)
    rapids_export(BUILD grpc
                  EXPORT_SET gRPCTargets
                  GLOBAL_TARGETS
                    grpc
                    grpc_cpp_plugin
                  NAMESPACE gRPC::)
  endif()

  if(TARGET gpr AND NOT TARGET gRPC::gpr)
    add_library(gRPC::gpr ALIAS gpr)
  endif()

  if(TARGET grpc AND NOT TARGET gRPC::grpc)
    add_library(gRPC::grpc ALIAS grpc)
  endif()

  if(TARGET grpc++ AND NOT TARGET gRPC::grpc++)
    add_library(gRPC::grpc++ ALIAS grpc++)
  endif()

  if(TARGET grpc_cpp_plugin AND NOT TARGET gRPC::grpc_cpp_plugin)
    add_executable(gRPC::grpc_cpp_plugin ALIAS grpc_cpp_plugin)
  endif()

  rapids_export_package(BUILD gRPC ${PROJECT_NAME}-core-exports)

  include("${rapids-cmake-dir}/export/find_package_root.cmake")
  rapids_export_find_package_root(BUILD gRPC [=[${CMAKE_CURRENT_LIST_DIR}]=] ${PROJECT_NAME}-core_exports)

endfunction()

find_and_configure_gRPC("1.48.0")
