# CoS-SSA: Context-Sensitive Interprocedural SSA for Global Variables

### Reproduce Evaluation

The build folder contains a pre-built binary.

```bash
make test # run unit tests
make eval # run evaluation
```

### Clean Build

```bash
make cleanall # remove all build artifacts and evaluation results
make build    # build the binary
```

### Directory Structure

- `src/` contains our implementation, split into more general code.
  - `src/llvm/` contains code for extract CFG and destroy CoS-SSA.
  - `src/cfg/` contains our CFG data structures.
  - `src/dfg/` contains code to compute and optimize the DFG.
  - `src/ssa/` contains our SSA data structures and code to generate CoS-SSA.
- `include` contains the header files corresponding to the `.cc` files in `src`.
- `docker` contains files to build/run `cos_ssa` in a container.
- `tests/cfg-to-ssa` contains our unit tests for DFG construction, optimization, and CoS-SSA generation.
- `test/e2e` contains our `.ll` unit tests for the entire program.
- `benchmarks` contains the benchmarks used in our evaluation.
- `scripts` contains build and run scripts used by our `Makefile`.
- `build` contains the pre-built binary.

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
