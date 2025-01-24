#/bin/sh

make DEBUG=0

if [[ -f cs_ssa ]]; then
    echo "Compilation Done!"
else
    echo "Compilation Failed!"
fi
