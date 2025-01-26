#/bin/sh

make DEBUG=0

if [[ -f cos_ssa ]]; then
    # CFG Tests
    ./cos_ssa --tool=cfg tests/0.cfg
    ./cos_ssa --tool=cfg tests/1.cfg
    ./cos_ssa --tool=cfg tests/2.cfg
    ./cos_ssa --tool=cfg tests/3.cfg
    ./cos_ssa --tool=cfg tests/4.cfg
    ./cos_ssa --tool=cfg tests/5.cfg
    ./cos_ssa --tool=cfg tests/6.cfg
    ./cos_ssa --tool=cfg tests/7_orig.cfg
    ./cos_ssa --tool=cfg tests/7_dupl.cfg
    ./cos_ssa --tool=cfg tests/8.cfg
    ./cos_ssa --tool=cfg tests/9.cfg
    ./cos_ssa --tool=cfg tests/10.cfg

    # SSA Tests
    ./cos_ssa --tool=ssa tests/0.ssa
    ./cos_ssa --tool=ssa tests/6.ssa
    ./cos_ssa --tool=ssa tests/7_orig.ssa
    ./cos_ssa --tool=ssa tests/7_exp.ssa
else
    echo "Compilation Failed!"
fi
