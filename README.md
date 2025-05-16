# CoS-SSA

### Build and Run Tests

Run

```bash
make cleanall
make build
make test
```

On the EWS machines, make sure the `LLVM` module is loaded before running build.

### Reproduce Evaluation

Run

```bash
make eval
```

### Repository Layout

`src` contains our implementation, split into more general code.

`llvm` contains code for extract CFG and destroy CoS-SSA.

`cfg` contains our CFG data structures.

`dfg` contains code to compute and optimize the DFG

`ssa` contains our SSA data structures and code to generate CoS-SSA.

`include` contains the header files corresponding to the `.cc` files in `src`.

`docker` contains files to build / run `cos_ssa` in a container.

`scripts` contains build and run scripts used by our `Makefile`.

`tests/cfg-to-ssa` contains our unit tests for DFG construction, optimization, and CoS-SSA generation.

`test/e2e` contains our `.ll` unit tests for the entire program.

### Running the Program

The `cos_ssa` binary is located under `build`. To run it on a `.ll` file and output a new `.ll` file, run

```bash
./build/cos_ssa --tool=all <target.ll>
```

`cos_ssa` can also be used to construct a `.cfg` file from a `.ll` file with

```bash
./build/cos_ssa --tool=llvm <target.ll>
```

This `.cfg` can be visualized with

```bash
./build/cos_ssa --tool=cfg <target.cfg>
```

The DFG for this `.cfg` can be dumped to the terminal with

```bash
./build/cos_ssa --tool=dfg <target.cfg>
```

The `.cfg` can be converted to a `.ssa` file with

```bash
./build/cos_ssa --tool=cfg-to-ssa <target.cfg>
```

Finally, this `.ssa` can be visualized with

```bash
./build/cos_ssa --tool=ssa <target.ssa>
```
