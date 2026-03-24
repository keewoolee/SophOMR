# SophOMR

This is a proof of concept implementation of SophOMR, an oblivious message retrieval scheme.

### Contributors
- Keewoo Lee (Ethereum Foundation)
- Yongdong Yeo (Seoul National University)

## To Build 

(based on Ubuntu 24.04 LTS with x86-64)

### Dependencies
- C++ build environment
- CMake build infrastructure
- [NTL](https://libntl.org/) library
- [OpenFHE](https://github.com/openfheorg/openfhe-development) library (tested with v1.4.0)
- [HEXL](https://github.com/intel/hexl) library (Optional; Skip on ARM) 
  - HEXL backend is optimized for processors with AVX512_IFMA support, e.g., Intel IceLake.

⚠️ To implement ring-switching, we use OpenFHE in a manner not officially supported by its APIs, which may be incompatible with OpenFHE versions beyond 1.4.0.

### Scripts to install the dependencies and build the library

1. Install CMake, GMP, and NTL (if needed).

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libgmp3-dev libntl-dev
```

2. Install [OpenFHE + HEXL](https://github.com/openfheorg/openfhe-hexl) by following the instruction in the link, or run:

```bash
git clone https://github.com/openfheorg/openfhe-configurator.git
cd openfhe-configurator
scripts/configure.sh

# Would you like to stage an openfhe-development build?     [y/n] : n  (y on ARM)
# Would you like to stage an openfhe-hexl build?            [y/n] : y

sudo scripts/build-openfhe-development.sh
```

3. Build the library.

```bash
cd ..  # if still in openfhe-configurator directory
git clone https://github.com/keewoolee/SophOMR.git  # clone this repository
cd SophOMR
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=~/openfhe-configurator/openfhe-staging/install # adjust the path to the location of the openfhe libraries
make
```

4. Basic test. This runs the fastest predefined configuration (1-2 compute-minutes). On success, the final line of the output will be: `Result is Correct!`

```bash
./test OMD 65536 16
```

## To Run

- To run all benchmarks presented in the paper (6-7 compute-hours):
```bash
python3 -u ../benchmark.py > benchmark.txt 2>&1
```

- To run with predefined parameters:
```bash
# ./test OMR <N> <k> <t>    (OMR requires payload size t)
# ./test OMD <N> <k>        (OMD has no payload)
# N: number of transactions
# k: number of pertinent messages
# t: payload size parameter

# Default configurations
./test OMR 65536 50 250
./test OMD 65536 50
./test OMR 524288 50 250
./test OMD 524288 50

# OMR: Varying t
./test OMR 65536 50 64
./test OMR 65536 50 128
./test OMR 65536 50 256
./test OMR 65536 50 512

# OMR: Varying k
./test OMR 65536 16 250
./test OMR 65536 32 250
./test OMR 65536 64 250
./test OMR 65536 128 250

# OMD: Varying k
./test OMD 65536 16
./test OMD 65536 32
./test OMD 65536 64
./test OMD 65536 128
```

### Custom parameters

To run with custom parameters, modify the values in `src/global.cpp` and rebuild:

```bash
./test
```

⚠️ Custom parameters may result in incorrect, insecure, or inefficient outcomes. Only recommended if you are sufficiently knowledgeable.
