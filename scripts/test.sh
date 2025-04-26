#!/bin/bash

# Set the working directory
cd "$(dirname "$0")"/..

# Run the tests
if [[ -f ./build/cos_ssa ]]; then
    # CFG Tests
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

    # SSA Tests
    ./build/cos_ssa --tool=ssa tests/0.ssa
    ./build/cos_ssa --tool=ssa tests/6.ssa
    ./build/cos_ssa --tool=ssa tests/7_orig.ssa
    ./build/cos_ssa --tool=ssa tests/7_exp.ssa
else
    echo "Build not found. Please run 'make build' to build the project."
    exit 1
fi
