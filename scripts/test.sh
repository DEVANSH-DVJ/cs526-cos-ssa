#!/bin/bash

# Set the working directory
cd "$(dirname "$0")"/..

# Run the tests
if [[ -f ./build/cos_ssa ]]; then
    echo "Running CFG Visualization Tests"
    ./build/cos_ssa --tool=cfg tests/0.cfg
    ./build/cos_ssa --tool=cfg tests/1.cfg
    ./build/cos_ssa --tool=cfg tests/2.cfg
    ./build/cos_ssa --tool=cfg tests/3.cfg
    ./build/cos_ssa --tool=cfg tests/4.cfg
    ./build/cos_ssa --tool=cfg tests/5.cfg
    ./build/cos_ssa --tool=cfg tests/6.cfg
    ./build/cos_ssa --tool=cfg tests/7_orig.cfg
    ./build/cos_ssa --tool=cfg tests/7_dupl.cfg
    ./build/cos_ssa --tool=cfg tests/8.cfg
    ./build/cos_ssa --tool=cfg tests/9.cfg
    ./build/cos_ssa --tool=cfg tests/10.cfg
    ./build/cos_ssa --tool=cfg tests/11.cfg
    ./build/cos_ssa --tool=cfg tests/12.cfg

    echo "Running SSA Visualization Tests"
    ./build/cos_ssa --tool=ssa tests/0.exp.ssa
    ./build/cos_ssa --tool=ssa tests/6.exp.ssa
    ./build/cos_ssa --tool=ssa tests/7_orig.exp.ssa
    ./build/cos_ssa --tool=ssa tests/7_exp.ssa

    echo "Running CoS-SSA Generation Tests"
    ./build/cos_ssa --tool=cfg-to-ssa tests/0.cfg
    ./build/cos_ssa --tool=ssa tests/0.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/1.cfg
    ./build/cos_ssa --tool=ssa tests/1.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/2.cfg
    ./build/cos_ssa --tool=ssa tests/2.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/3.cfg
    ./build/cos_ssa --tool=ssa tests/3.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/4.cfg
    ./build/cos_ssa --tool=ssa tests/4.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/5.cfg
    ./build/cos_ssa --tool=ssa tests/5.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/6.cfg
    ./build/cos_ssa --tool=ssa tests/6.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/7_orig.cfg
    ./build/cos_ssa --tool=ssa tests/7_orig.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/7_dupl.cfg
    ./build/cos_ssa --tool=ssa tests/7_dupl.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/8.cfg
    ./build/cos_ssa --tool=ssa tests/8.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/9.cfg
    ./build/cos_ssa --tool=ssa tests/9.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/10.cfg
    ./build/cos_ssa --tool=ssa tests/10.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/11.cfg
    ./build/cos_ssa --tool=ssa tests/11.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/12.cfg
    ./build/cos_ssa --tool=ssa tests/12.ssa

    echo "Running E2E LLVM IR Tests"
    ./build/cos_ssa --tool=all tests/e2e/constant_propagation.ll
    ./build/cos_ssa --tool=all tests/e2e/dead_code_elimination.ll
    ./build/cos_ssa --tool=all tests/e2e/input.ll
    ./build/cos_ssa --tool=all tests/e2e/metamorphic_assignment.ll
    ./build/cos_ssa --tool=all tests/e2e/cos_ssa_phi.ll
    ./build/cos_ssa --tool=all tests/e2e/simple_return.ll
    ./build/cos_ssa --tool=all tests/e2e/metamorphic_return.ll
    ./build/cos_ssa --tool=all tests/e2e/multiple_partitions.ll
else
    echo "Build not found. Please run './scripts/build.sh' to build the project."
    exit 1
fi
