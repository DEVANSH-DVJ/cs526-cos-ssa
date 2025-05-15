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

    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/dds7.ll > benchmarks/dds7.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/dijkstra_large.ll > benchmarks/dijkstra_large.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/enchantedmaze.ll > benchmarks/enchantedmaze.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/int-stc.ll > benchmarks/int-stc.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/int-stc2.ll  > benchmarks/int-stc2.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/int-stcp.ll  > benchmarks/int-stcp.log 2>&1
    /usr/bin/time -v ./build/cos_ssa --tool=all benchmarks/shell.ll > benchmarks/shell.log 2>&1

    clang -O3 -Wno-deprecated-builtins benchmarks/dds7.ll -o benchmarks/dds7.old.o # warning
    clang -O3 benchmarks/dijkstra_large.ll -o benchmarks/dijkstra_large.old.o
    clang++ -O3 benchmarks/enchantedmaze.ll -o benchmarks/enchantedmaze.old.o
    clang -O3 -lm benchmarks/int-stc.ll -o benchmarks/int-stc.old.o
    clang -O3 -lm benchmarks/int-stc2.ll -o benchmarks/int-stc2.old.o
    clang++ -O3 benchmarks/int-stcp.ll -o benchmarks/int-stcp.old.o
    # clang -O3 benchmarks/shell.ll -o benchmarks/shell.old.o # error

    clang -O3 -Wno-deprecated-builtins benchmarks/dds7.out.ll -o benchmarks/dds7.new.o # warning
    clang -O3 benchmarks/dijkstra_large.out.ll -o benchmarks/dijkstra_large.new.o
    clang++ -O3 benchmarks/enchantedmaze.out.ll -o benchmarks/enchantedmaze.new.o
    # clang -O3 -lm benchmarks/int-stc.out.ll -o benchmarks/int-stc.new.o # error
    # clang -O3 -lm benchmarks/int-stc2.out.ll -o benchmarks/int-stc2.new.o # error
    # clang++ -O3 benchmarks/int-stcp.out.ll -o benchmarks/int-stcp.new.o # error
    # clang -O3 benchmarks/shell.out.ll -o benchmarks/shell.new.o # error
else
    echo "Build not found. Please run 'make build' to build the project."
    exit 1
fi
