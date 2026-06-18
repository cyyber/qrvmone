# qrvmone

[![ethereum badge]][ethereum]
[![readme style standard badge]][standard readme]
[![codecov badge]][codecov]
[![circleci badge]][circleci]
[![appveyor badge]][appveyor]
[![license badge]][Apache License, Version 2.0]

> Fast Quantum Resistant Virtual Machine implementation

_qrvmone_ is a C++ implementation of the Quantum Resistant Virtual Machine (QRVM). 
Created by members of the [Ipsilon] (ex-[Ewasm]) team, the project aims for clean, standalone QRVM implementation 
that can be imported as an execution module by QRL Client projects. 
The codebase of _qrvmone_ is optimized to provide fast and efficient execution of QRVM smart contracts.

### Characteristic of qrvmone

1. Exposes the [QRVMC] API.
2. Requires C++20 standard.
3. The [intx] library is used to provide multi-precision integer arithmetic (256-bit and 512-bit) used by the QRVM stack.
4. The [ethash] library is used to provide Keccak hash function implementation
   needed for the special `KECCAK256` instruction.
5. Contains two interpreters: 
   - **Baseline** (default)
   - **Advanced** (select with the `advanced` option)

### Baseline Interpreter

1. Provides relatively straight-forward but efficient QRVM implementation.
2. Performs only minimalistic `JUMPDEST` analysis.

### Advanced Interpreter

1. The _indirect call threading_ is the dispatch method used -
   a loaded QRVM program is a table with pointers to functions implementing virtual instructions.
2. The gas cost and stack requirements of block of instructions is precomputed 
   and applied once per block during execution.
3. Performs extensive and expensive bytecode analysis before execution.


## Usage

### As gzond plugin

qrvmone implements the [QRVMC] API for Quantum Resistant Virtual Machines.
It can be used as a plugin replacing gzond's internal QRVM. But for that a modified
version of gzond is needed. The [Ewasm]'s fork
of go-zond provides [binary releases of gzond with QRVMC support](https://github.com/ewasm/go-ethereum/releases).

Next, download qrvmone from [Releases].

Start the downloaded gzond with `--vm.qrvm` option pointing to the qrvmone shared library.

```bash
gzond --vm.qrvm=./libqrvmone.so
```

### Building from source

To build the qrvmone QRVMC module (shared library), test, and benchmark:

1. Fetch the source code:
   ```
   git clone --recursive https://github.com/theqrl/qrvmone
   cd qrvmone
   ```

2. Configure the project build and dependencies:
   ##### Linux / OSX
   ```
   cmake -S . -B build -DQRVMONE_TESTING=ON
   ```

   ##### Windows
   ```
   cmake -S . -B build -DQRVMONE_TESTING=ON -G "Visual Studio 16 2019" -A x64
   ```
   
3. Build:
   ```
   cmake --build build --parallel
   ```


3. Run the unit tests or benchmarking tool:
   ```
   build/bin/qrvmone-unittests
   build/bin/qrvmone-bench test/qrvm-benchmarks/benchmarks
   ```

### Tools

#### qrvm-test

The **qrvm-test** executes a collection of unit tests on 
any QRVMC-compatible Quantum Resistant Virtual Machine implementation.
The collection of tests comes from the qrvmone project.

```bash
qrvm-test ./qrvmone.so
```

### Docker

Docker images with qrvmone are available on Docker Hub:
https://hub.docker.com/r/qrledger/qrvmone.

Having the qrvmone shared library inside a docker is not very useful on its own,
but the image can be used as the base of another one or you can run benchmarks 
with it.

```bash
docker run --entrypoint qrvmone-bench qrledger/qrvmone /src/test/benchmarks
```

## References

1. [Efficient gas calculation algorithm for QRVM](docs/efficient_gas_calculation_algorithm.md)

## Maintainer

Paweł Bylica [@chfast]

## License

[![license badge]][Apache License, Version 2.0]

Licensed under the [Apache License, Version 2.0].