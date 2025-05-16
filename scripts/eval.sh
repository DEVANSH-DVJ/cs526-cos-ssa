#!/bin/bash

module load llvm/15.0.7

# Set the working directory
cd "$(dirname "$0")"/..
ulimit -Sv 8388608
ulimit -t 60

# Run the tests
if [[ -f ./build/cos_ssa ]]; then
    clang -S -Wno-deprecated-builtins -emit-llvm -o benchmarks/dds7.ll benchmarks/dds7.c
    clang -S -Wno-return-type -emit-llvm -o benchmarks/dijkstra_large.ll benchmarks/dijkstra_large.c
    clang++ -S -emit-llvm -o benchmarks/enchantedmaze.ll benchmarks/enchantedmaze.cpp
    clang -S -emit-llvm -o benchmarks/int-stc.ll benchmarks/int-stc.c
    clang -S -emit-llvm -o benchmarks/int-stc2.ll benchmarks/int-stc2.c
    clang++ -S -emit-llvm -o benchmarks/int-stcp.ll benchmarks/int-stcp.cpp
    clang -S -emit-llvm -o benchmarks/shell.ll benchmarks/shell.c

    /usr/bin/time -v ./build/cos_ssa --tool=all --debug benchmarks/dds7.ll >benchmarks/dds7.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all --debug benchmarks/dijkstra_large.ll >benchmarks/dijkstra_large.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all --debug benchmarks/enchantedmaze.ll >benchmarks/enchantedmaze.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all --debug benchmarks/int-stc.ll >benchmarks/int-stc.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all --debug benchmarks/int-stc2.ll >benchmarks/int-stc2.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all --debug benchmarks/int-stcp.ll >benchmarks/int-stcp.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all --debug benchmarks/shell.ll >benchmarks/shell.log 2>&1

    opt -passes=ipsccp,globalopt,globaldce benchmarks/dds7.ll -S -o benchmarks/dds7.baseline.ll
    opt -passes=ipsccp,globalopt,globaldce benchmarks/dijkstra_large.ll -S -o benchmarks/dijkstra_large.baseline.ll
    opt -passes=ipsccp,globalopt,globaldce benchmarks/enchantedmaze.ll -S -o benchmarks/enchantedmaze.baseline.ll
    opt -passes=ipsccp,globalopt,globaldce benchmarks/int-stc.ll -S -o benchmarks/int-stc.baseline.ll
    opt -passes=ipsccp,globalopt,globaldce benchmarks/int-stc2.ll -S -o benchmarks/int-stc2.baseline.ll
    opt -passes=ipsccp,globalopt,globaldce benchmarks/int-stcp.ll -S -o benchmarks/int-stcp.baseline.ll
    opt -passes=ipsccp,globalopt,globaldce benchmarks/shell.ll -S -o benchmarks/shell.baseline.ll

    python3 scripts/summarize.py
else
    echo "Build not found. Please run 'make build' to build the project."
    exit 1
fi
