#!/bin/bash

module load llvm/15.0.7

# Set the working directory
cd "$(dirname "$0")"/..

# Run the tests
if [[ -f ./build/cos_ssa ]]; then
    # CFG Visualization Tests
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

    # E2E Tests (w/ opt)
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

    # E2E Tests (w/o opt)
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

    # SSA Visualization Tests
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
