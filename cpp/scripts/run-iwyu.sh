#!/bin/bash

set -euo pipefail

if [[ -f "./build/compile_commands.json" ]]; then
   # typical build
   build_dir="./build";
elif [[ -f "./build/latest/compile_commands.json" ]]; then
   # rapids build utils build
   build_dir="./build/latest";
else
   echo "unable to find compile_commands.json";
   exit 1;
fi

iwyu_tool.py -p $build_dir "$@" -- \
   -Xiwyu --mapping_file=$PWD/ci/iwyu/mappings.imp \
   -Xiwyu --verbose=${IWYU_VERBOSITY:-1} \
   -Xiwyu --quoted_includes_first \
   -Xiwyu --cxx17ns \
   -Xiwyu --error=1 \
   --driver-mode=g++ | fix_includes.py

echo "hey" > tmp.txt
