#!/bin/bash

module load llvm/15.0.7

# Set the working directory
cd "$(dirname "$0")"/..

# Run the tests
if [[ -f ./build/cos_ssa ]]; then
    # CFG Visualization Tests
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

    # E2E Tests (w/ opt)
    ./build/cos_ssa --tool=cfg-to-ssa tests/0.cfg && mv tests/0.ssa tests/0.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/1.cfg && mv tests/1.ssa tests/1.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/2.cfg && mv tests/2.ssa tests/2.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/3.cfg && mv tests/3.ssa tests/3.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/4.cfg && mv tests/4.ssa tests/4.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/5.cfg && mv tests/5.ssa tests/5.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/6.cfg && mv tests/6.ssa tests/6.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/7_orig.cfg && mv tests/7_orig.ssa tests/7_orig.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/7_dupl.cfg && mv tests/7_dupl.ssa tests/7_dupl.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/8.cfg && mv tests/8.ssa tests/8.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/9.cfg && mv tests/9.ssa tests/9.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/10.cfg && mv tests/10.ssa tests/10.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/11.cfg && mv tests/11.ssa tests/11.opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa tests/12.cfg && mv tests/12.ssa tests/12.opt.ssa

    # E2E Tests (w/o opt)
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/0.cfg && mv tests/0.ssa tests/0.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/1.cfg && mv tests/1.ssa tests/1.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/2.cfg && mv tests/2.ssa tests/2.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/3.cfg && mv tests/3.ssa tests/3.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/4.cfg && mv tests/4.ssa tests/4.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/5.cfg && mv tests/5.ssa tests/5.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/6.cfg && mv tests/6.ssa tests/6.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/7_orig.cfg && mv tests/7_orig.ssa tests/7_orig.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/7_dupl.cfg && mv tests/7_dupl.ssa tests/7_dupl.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/8.cfg && mv tests/8.ssa tests/8.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/9.cfg && mv tests/9.ssa tests/9.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/10.cfg && mv tests/10.ssa tests/10.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/11.cfg && mv tests/11.ssa tests/11.no-opt.ssa
    ./build/cos_ssa --tool=cfg-to-ssa --no-opt tests/12.cfg && mv tests/12.ssa tests/12.no-opt.ssa

    # SSA Visualization Tests
    ./build/cos_ssa --tool=ssa tests/0.opt.ssa
    ./build/cos_ssa --tool=ssa tests/1.opt.ssa
    ./build/cos_ssa --tool=ssa tests/2.opt.ssa
    ./build/cos_ssa --tool=ssa tests/3.opt.ssa
    ./build/cos_ssa --tool=ssa tests/4.opt.ssa
    ./build/cos_ssa --tool=ssa tests/5.opt.ssa
    ./build/cos_ssa --tool=ssa tests/6.opt.ssa
    ./build/cos_ssa --tool=ssa tests/7_orig.opt.ssa
    ./build/cos_ssa --tool=ssa tests/7_dupl.opt.ssa
    ./build/cos_ssa --tool=ssa tests/8.opt.ssa
    ./build/cos_ssa --tool=ssa tests/9.opt.ssa
    ./build/cos_ssa --tool=ssa tests/10.opt.ssa
    ./build/cos_ssa --tool=ssa tests/11.opt.ssa
    ./build/cos_ssa --tool=ssa tests/12.opt.ssa
    ./build/cos_ssa --tool=ssa tests/0.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/1.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/2.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/3.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/4.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/5.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/6.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/7_orig.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/7_dupl.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/8.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/9.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/10.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/11.no-opt.ssa
    ./build/cos_ssa --tool=ssa tests/12.no-opt.ssa

    # Running E2E LLVM IR Tests
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
