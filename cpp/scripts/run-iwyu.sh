#!/bin/bash

if [[ -f "./build/compile_commands.json" ]]; then
   build_dir="./build";
elif [[ -f "./build/latest/compile_commands.json" ]]; then
   build_dir="./build/latest";
else
   exit 1;
fi

# echo "files: $@"
# echo ""
# exit 1;

iwyu_tool.py -p $build_dir "$@" -- \
   -Xiwyu --mapping_file=$PWD/ci/iwyu/mappings.imp \
   -Xiwyu --verbose=${IWYU_VERBOSITY:-1} \
   -Xiwyu --quoted_includes_first \
   -Xiwyu --cxx17ns \
   -Xiwyu --error=1 \
   --driver-mode=g++
