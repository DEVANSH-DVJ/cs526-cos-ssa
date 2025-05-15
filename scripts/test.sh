#!/bin/bash

module load llvm/15.0.7

# Set the working directory
cd "$(dirname "$0")"/..

# Run the tests
if [[ -f ./build/cos_ssa ]]; then
    # CFG to SSA Tests
    echo "----------------------------------------"
    echo "Running CFG to SSA tests..."

    # CFG Visualization Tests
    echo "Visualizing CFG..."
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/0.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/1.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/2.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/3.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/4.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/5.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/6.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/7_orig.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/7_dupl.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/8.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/9.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/10.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/11.cfg
    ./build/cos_ssa --tool=cfg tests/cfg-to-ssa/12.cfg

    # SSA (w/o opt)
    echo "Generating SSA before optimizations..."
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/0.cfg && mv tests/cfg-to-ssa/0.ssa tests/cfg-to-ssa/0.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/1.cfg && mv tests/cfg-to-ssa/1.ssa tests/cfg-to-ssa/1.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/2.cfg && mv tests/cfg-to-ssa/2.ssa tests/cfg-to-ssa/2.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/3.cfg && mv tests/cfg-to-ssa/3.ssa tests/cfg-to-ssa/3.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/4.cfg && mv tests/cfg-to-ssa/4.ssa tests/cfg-to-ssa/4.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/5.cfg && mv tests/cfg-to-ssa/5.ssa tests/cfg-to-ssa/5.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/6.cfg && mv tests/cfg-to-ssa/6.ssa tests/cfg-to-ssa/6.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/7_orig.cfg && mv tests/cfg-to-ssa/7_orig.ssa tests/cfg-to-ssa/7_orig.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/7_dupl.cfg && mv tests/cfg-to-ssa/7_dupl.ssa tests/cfg-to-ssa/7_dupl.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/8.cfg && mv tests/cfg-to-ssa/8.ssa tests/cfg-to-ssa/8.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/9.cfg && mv tests/cfg-to-ssa/9.ssa tests/cfg-to-ssa/9.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/10.cfg && mv tests/cfg-to-ssa/10.ssa tests/cfg-to-ssa/10.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/11.cfg && mv tests/cfg-to-ssa/11.ssa tests/cfg-to-ssa/11.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/cfg-to-ssa/12.cfg && mv tests/cfg-to-ssa/12.ssa tests/cfg-to-ssa/12.no-opt.ssa

    # SSA (w/ opt)
    echo "Generating SSA after optimizations..."
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/0.cfg && mv tests/cfg-to-ssa/0.ssa tests/cfg-to-ssa/0.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/1.cfg && mv tests/cfg-to-ssa/1.ssa tests/cfg-to-ssa/1.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/2.cfg && mv tests/cfg-to-ssa/2.ssa tests/cfg-to-ssa/2.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/3.cfg && mv tests/cfg-to-ssa/3.ssa tests/cfg-to-ssa/3.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/4.cfg && mv tests/cfg-to-ssa/4.ssa tests/cfg-to-ssa/4.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/5.cfg && mv tests/cfg-to-ssa/5.ssa tests/cfg-to-ssa/5.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/6.cfg && mv tests/cfg-to-ssa/6.ssa tests/cfg-to-ssa/6.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/7_orig.cfg && mv tests/cfg-to-ssa/7_orig.ssa tests/cfg-to-ssa/7_orig.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/7_dupl.cfg && mv tests/cfg-to-ssa/7_dupl.ssa tests/cfg-to-ssa/7_dupl.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/8.cfg && mv tests/cfg-to-ssa/8.ssa tests/cfg-to-ssa/8.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/9.cfg && mv tests/cfg-to-ssa/9.ssa tests/cfg-to-ssa/9.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/10.cfg && mv tests/cfg-to-ssa/10.ssa tests/cfg-to-ssa/10.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/11.cfg && mv tests/cfg-to-ssa/11.ssa tests/cfg-to-ssa/11.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/cfg-to-ssa/12.cfg && mv tests/cfg-to-ssa/12.ssa tests/cfg-to-ssa/12.opt.ssa

    # SSA Visualization Tests
    echo "Visualizing SSA..."
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/0.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/1.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/2.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/3.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/4.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/5.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/6.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/7_orig.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/7_dupl.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/8.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/9.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/10.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/11.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/12.opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/0.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/1.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/2.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/3.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/4.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/5.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/6.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/7_orig.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/7_dupl.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/8.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/9.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/10.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/11.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/cfg-to-ssa/12.no-opt.ssa

    echo "CFG to SSA Tests Complete"
    echo "----------------------------------------"

    # E2E Tests
    echo "----------------------------------------"
    echo "Running E2E tests..."

    # constant_propagation
    clang -S -emit-llvm -o tests/e2e/constant_propagation.ll tests/e2e/constant_propagation.c
    ./build/cos_ssa --tool=all tests/e2e/constant_propagation.ll
    clang -O0 tests/e2e/constant_propagation.ll -o tests/e2e/constant_propagation.old.o
    clang -O0 tests/e2e/constant_propagation.out.ll -o tests/e2e/constant_propagation.new.o
    ./tests/e2e/constant_propagation.old.o >tests/e2e/constant_propagation.old.stdout
    ./tests/e2e/constant_propagation.new.o >tests/e2e/constant_propagation.new.stdout
    diff tests/e2e/constant_propagation.old.stdout tests/e2e/constant_propagation.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: constant_propagation"
    else
        echo "E2E Test Fail: constant_propagation"
        exit 1
    fi

    # cos_ssa_phi
    clang -S -emit-llvm -o tests/e2e/cos_ssa_phi.ll tests/e2e/cos_ssa_phi.c
    ./build/cos_ssa --tool=all tests/e2e/cos_ssa_phi.ll
    clang -O0 tests/e2e/cos_ssa_phi.ll -o tests/e2e/cos_ssa_phi.old.o
    clang -O0 tests/e2e/cos_ssa_phi.out.ll -o tests/e2e/cos_ssa_phi.new.o
    ./tests/e2e/cos_ssa_phi.old.o >tests/e2e/cos_ssa_phi.old.stdout
    ./tests/e2e/cos_ssa_phi.new.o >tests/e2e/cos_ssa_phi.new.stdout
    diff tests/e2e/cos_ssa_phi.old.stdout tests/e2e/cos_ssa_phi.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: cos_ssa_phi"
    else
        echo "E2E Test Fail: cos_ssa_phi"
        exit 1
    fi

    # dead_code_elimination
    clang -S -emit-llvm -o tests/e2e/dead_code_elimination.ll tests/e2e/dead_code_elimination.c
    ./build/cos_ssa --tool=all tests/e2e/dead_code_elimination.ll
    clang -O0 tests/e2e/dead_code_elimination.ll -o tests/e2e/dead_code_elimination.old.o
    clang -O0 tests/e2e/dead_code_elimination.out.ll -o tests/e2e/dead_code_elimination.new.o
    ./tests/e2e/dead_code_elimination.old.o >tests/e2e/dead_code_elimination.old.stdout
    ./tests/e2e/dead_code_elimination.new.o >tests/e2e/dead_code_elimination.new.stdout
    diff tests/e2e/dead_code_elimination.old.stdout tests/e2e/dead_code_elimination.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: dead_code_elimination"
    else
        echo "E2E Test Fail: dead_code_elimination"
        exit 1
    fi

    # input
    clang -S -emit-llvm -o tests/e2e/input.ll tests/e2e/input.c
    ./build/cos_ssa --tool=all tests/e2e/input.ll
    clang -O0 tests/e2e/input.ll -o tests/e2e/input.old.o
    clang -O0 tests/e2e/input.out.ll -o tests/e2e/input.new.o
    ./tests/e2e/input.old.o >tests/e2e/input.old.stdout
    ./tests/e2e/input.new.o >tests/e2e/input.new.stdout
    diff tests/e2e/input.old.stdout tests/e2e/input.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: input"
    else
        echo "E2E Test Fail: input"
        exit 1
    fi

    # metamorphic_assignment
    clang -S -emit-llvm -o tests/e2e/metamorphic_assignment.ll tests/e2e/metamorphic_assignment.c
    ./build/cos_ssa --tool=all tests/e2e/metamorphic_assignment.ll
    clang -O0 tests/e2e/metamorphic_assignment.ll -o tests/e2e/metamorphic_assignment.old.o
    clang -O0 tests/e2e/metamorphic_assignment.out.ll -o tests/e2e/metamorphic_assignment.new.o
    ./tests/e2e/metamorphic_assignment.old.o >tests/e2e/metamorphic_assignment.old.stdout
    ./tests/e2e/metamorphic_assignment.new.o >tests/e2e/metamorphic_assignment.new.stdout
    diff tests/e2e/metamorphic_assignment.old.stdout tests/e2e/metamorphic_assignment.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: metamorphic_assignment"
    else
        echo "E2E Test Fail: metamorphic_assignment"
        exit 1
    fi

    # metamorphic_return
    clang -S -emit-llvm -o tests/e2e/metamorphic_return.ll tests/e2e/metamorphic_return.c
    ./build/cos_ssa --tool=all tests/e2e/metamorphic_return.ll
    clang -O0 tests/e2e/metamorphic_return.ll -o tests/e2e/metamorphic_return.old.o
    clang -O0 tests/e2e/metamorphic_return.out.ll -o tests/e2e/metamorphic_return.new.o
    ./tests/e2e/metamorphic_return.old.o >tests/e2e/metamorphic_return.old.stdout
    ./tests/e2e/metamorphic_return.new.o >tests/e2e/metamorphic_return.new.stdout
    diff tests/e2e/metamorphic_return.old.stdout tests/e2e/metamorphic_return.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: metamorphic_return"
    else
        echo "E2E Test Fail: metamorphic_return"
        exit 1
    fi

    # motivating_example
    clang -S -emit-llvm -o tests/e2e/motivating_example.ll tests/e2e/motivating_example.c
    ./build/cos_ssa --tool=all tests/e2e/motivating_example.ll
    clang -O0 tests/e2e/motivating_example.ll -o tests/e2e/motivating_example.old.o
    clang -O0 tests/e2e/motivating_example.out.ll -o tests/e2e/motivating_example.new.o
    ./tests/e2e/motivating_example.old.o >tests/e2e/motivating_example.old.stdout
    ./tests/e2e/motivating_example.new.o >tests/e2e/motivating_example.new.stdout
    diff tests/e2e/motivating_example.old.stdout tests/e2e/motivating_example.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: motivating_example"
    else
        echo "E2E Test Fail: motivating_example"
        exit 1
    fi

    # multiple_partitions
    clang -S -emit-llvm -o tests/e2e/multiple_partitions.ll tests/e2e/multiple_partitions.c
    ./build/cos_ssa --tool=all tests/e2e/multiple_partitions.ll
    clang -O0 tests/e2e/multiple_partitions.ll -o tests/e2e/multiple_partitions.old.o
    clang -O0 tests/e2e/multiple_partitions.out.ll -o tests/e2e/multiple_partitions.new.o
    ./tests/e2e/multiple_partitions.old.o >tests/e2e/multiple_partitions.old.stdout
    ./tests/e2e/multiple_partitions.new.o >tests/e2e/multiple_partitions.new.stdout
    diff tests/e2e/multiple_partitions.old.stdout tests/e2e/multiple_partitions.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: multiple_partitions"
    else
        echo "E2E Test Fail: multiple_partitions"
        exit 1
    fi

    # simple_return
    clang -S -emit-llvm -o tests/e2e/simple_return.ll tests/e2e/simple_return.c
    ./build/cos_ssa --tool=all tests/e2e/simple_return.ll
    clang -O0 tests/e2e/simple_return.ll -o tests/e2e/simple_return.old.o
    clang -O0 tests/e2e/simple_return.out.ll -o tests/e2e/simple_return.new.o
    ./tests/e2e/simple_return.old.o >tests/e2e/simple_return.old.stdout
    ./tests/e2e/simple_return.new.o >tests/e2e/simple_return.new.stdout
    diff tests/e2e/simple_return.old.stdout tests/e2e/simple_return.new.stdout
    if [[ $? -eq 0 ]]; then
        echo "E2E Test Pass: simple_return"
    else
        echo "E2E Test Fail: simple_return"
        exit 1
    fi

    echo "E2E Tests Complete"
    echo "----------------------------------------"
else
    echo "Build not found. Please run './scripts/build.sh' to build the project."
    exit 1
fi
