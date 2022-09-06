#=============================================================================
# Copyright (c) 2021, NVIDIA CORPORATION.
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

function(find_and_configure_abseil version GIT_TAG)

    if(TARGET absl::time)
        return()
    endif()

    rapids_cpm_find(absl   ${version}
        GLOBAL_TARGETS     absl::bad_variant_access
                           absl::base
                           absl::civil_time
                           absl::int128
                           absl::raw_logging_internal
                           absl::spinlock_wait
                           absl::str_format_internal
                           absl::strings
                           absl::strings_internal
                           absl::time
                           absl::time_zone
        BUILD_EXPORT_SET   ${PROJECT_NAME}-core-exports
        INSTALL_EXPORT_SET ${PROJECT_NAME}-core-exports
        CPM_ARGS
            GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
            GIT_TAG        ${GIT_TAG}
            GIT_SHALLOW    TRUE
            OPTIONS        "ABSL_BUILD_TESTING OFF"
                           "ABSL_CC_LIB_TESTONLY OFF"
                           "ABSL_ENABLE_INSTALL ON"
    )

    if(absl_ADDED)
        export(EXPORT abslTargets
               NAMESPACE absl::
               FILE "${absl_BINARY_DIR}/abslTargets.cmake")
        set(absl_DIR "${absl_BINARY_DIR}" PARENT_SCOPE)
    endif()

endfunction()
find_and_configure_abseil("20220623" "20220623.0")
