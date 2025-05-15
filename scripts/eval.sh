#!/bin/bash

module load llvm/15.0.7

# Set the working directory
cd "$(dirname "$0")"/..
ulimit -Sv 8388608
ulimit -t 60

# Run the tests
if [[ -f ./build/cos_ssa ]]; then
    clang -S -emit-llvm -o benchmarks/dds7.ll benchmarks/dds7.c
    clang -S -emit-llvm -o benchmarks/dijkstra_large.ll benchmarks/dijkstra_large.c
    clang++ -S -emit-llvm -o benchmarks/enchantedmaze.ll benchmarks/enchantedmaze.cpp
    clang -S -emit-llvm -o benchmarks/int-stc.ll benchmarks/int-stc.c
    clang -S -emit-llvm -o benchmarks/int-stc2.ll benchmarks/int-stc2.c
    clang++ -S -emit-llvm -o benchmarks/int-stcp.ll benchmarks/int-stcp.cpp
    clang -S -emit-llvm -o benchmarks/shell.ll benchmarks/shell.c

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
