#!/bin/bash

# Set the working directory
cd "$(dirname "$0")"/..
ulimit -Sv 8388608
ulimit -t 60

# Run the tests
if [[ -f ./build/cos_ssa ]]; then
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/dds7.ll
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/dijkstra_large.ll
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/enchantedmaze.ll
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/int-stc.ll
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/int-stc2.ll
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/int-stcp.ll
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/shell.ll
else
    echo "Build not found. Please run 'make build' to build the project."
    exit 1
fi
