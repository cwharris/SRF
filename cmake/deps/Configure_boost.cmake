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

function(find_and_configure_boost version)

    list(APPEND CMAKE_MESSAGE_CONTEXT "boost")
    list(APPEND BOOST_INCLUDE_LIBRARIES context fiber)

    rapids_cpm_find(Boost ${version}
        GLOBAL_TARGETS              Boost::context
                                    Boost::core
                                    Boost::fiber
                                    Boost::filesystem
                                    Boost::hana
                                    Boost::system
        BUILD_EXPORT_SET            ${PROJECT_NAME}-core-exports
        INSTALL_EXPORT_SET          ${PROJECT_NAME}-core-exports
        CPM_ARGS
            GIT_REPOSITORY          https://github.com/boostorg/boost.git
            GIT_TAG                 "boost-${version}"
            GIT_SHALLOW             TRUE
            GIT_SUBMODULES          "libs/algorithm"
                                    "libs/align"
                                    "libs/array"
                                    "libs/assert"
                                    "libs/atomic"
                                    "libs/bind"
                                    "libs/concept_check"
                                    "libs/config"
                                    "libs/container_hash"
                                    "libs/context"
                                    "libs/conversion"
                                    "libs/core"
                                    "libs/detail"
                                    "libs/exception"
                                    "libs/fiber"
                                    "libs/filesystem"
                                    "libs/unordered"
                                    "libs/format"
                                    "libs/function_types"
                                    "libs/function"
                                    "libs/fusion"
                                    "libs/integer"
                                    "libs/intrusive"
                                    "libs/io"
                                    "libs/iterator"
                                    "libs/move"
                                    "libs/mp11"
                                    "libs/mpl"
                                    "libs/optional"
                                    "libs/pool"
                                    "libs/predef"
                                    "libs/preprocessor"
                                    "libs/range"
                                    "libs/regex"
                                    "libs/smart_ptr"
                                    "libs/static_assert"
                                    "libs/system"
                                    "libs/throw_exception"
                                    "libs/tuple"
                                    "libs/type_index"
                                    "libs/type_traits"
                                    "libs/typeof"
                                    "libs/utility"
                                    "libs/variant2"
                                    "libs/winapi"
                                    "tools/cmake"
            OPTIONS                 "BUILD_TESTING OFF"
            FIND_PACKAGE_ARGUMENTS  "COMPONENTS context fiber filesystem system"
    )

    
    if(Boost_ADDED)

        message(FATAL_ERROR "Boost-cmake is not yet supported. See https://github.com/boostorg/cmake for more details")

        # install(TARGETS boost_fiber
        #         EXPORT  boost_fiber-targets)
        # install(TARGETS boost_context
        #         EXPORT  boost_context-targets)
    endif()

endfunction()

# - Use static linking to avoid issues with system-wide installations of Boost.
# - Use numa=on to ensure the numa component of fiber gets built
find_and_configure_boost("1.74.0")
