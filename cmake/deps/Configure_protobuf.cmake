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

function(find_and_configure_protobuf VERSION)

  list(APPEND CMAKE_MESSAGE_CONTEXT "protobuf")

  rapids_cpm_find(protobuf ${VERSION}
    GLOBAL_TARGETS
      grpc
      grpc_cpp_plugin
      address_sorting
    #   gRPC::gpr
    #   gRPC::grpc_csharp_plugin
    #   gRPC::grpc_node_plugin
    #   gRPC::grpc_objective_c_plugin
    #   gRPC::grpc_php_plugin
    #   gRPC::grpc_plugin_support
    #   gRPC::grpc_python_plugin
    #   gRPC::grpc_ruby_plugin
    #   gRPC::grpc_unsecure
    #   gRPC::grpc++
    #   gRPC::grpc++_alts
    #   gRPC::grpc++_error_details
    #   gRPC::grpc++_reflection
    #   gRPC::grpc++_unsecure
    #   gRPC::grpcpp_channelz
    #   gRPC::upb
    BUILD_EXPORT_SET
      ${PROJECT_NAME}-core-exports
    INSTALL_EXPORT_SET
      ${PROJECT_NAME}-core-exports
    CPM_ARGS
      GIT_REPOSITORY  https://github.com/protocolbuffers/protobuf
      GIT_TAG         v${VERSION}
      GIT_SHALLOW     TRUE
      OPTIONS         "BUILD_TESTS OFF"
                      "BUILD_BENCHMARKS OFF"
                      "CUDA_STATIC_RUNTIME ON"
                      "DISABLE_DEPRECATION_WARNING ${DISABLE_DEPRECATION_WARNINGS}"
                      "protobuf_INSTALL ON"
    
    
  )

endfunction()

find_and_configure_protobuf(${PROTOBUF_VERSION})
