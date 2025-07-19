# SophOMR

This is a proof of concept implementation of SophOMR, an oblivious message retrieval scheme.

## To Build 

(based on Ubuntu 20.04 LTS)

### Dependencies
- C++ build environment
- CMake build infrastructure
- [NTL](https://libntl.org/) library
- [OpenFHE](https://github.com/openfheorg/openfhe-development) library (tested with v1.2.0)
- [HEXL](https://github.com/intel/hexl) library (HEXL backend is optimized for processors with AVX512_IFMA support, e.g., IceLake.)

⚠️ To implement ring-switching, we use OpenFHE in a manner not officially supported by its APIs, which may be incompatible with OpenFHE versions beyond 1.2.0.

### Scripts to install the dependencies and build the library

1. Install CMake, GMP, and NTL (if needed).

```
sudo apt-get update 
sudo apt-get install build-essential 
sudo apt-get install cmake 
sudo apt-get install libgmp3-dev 
sudo apt-get install libntl-dev 
```

2. Install [OpenFHE + HEXL](https://github.com/openfheorg/openfhe-hexl) by following the instruction in the link, or run:

```
git clone https://github.com/openfheorg/openfhe-configurator.git
cd openfhe-configurator
scripts/configure.sh

# Would you like to stage an openfhe-development build?     [y/n] : n
# Would you like to stage an openfhe-hexl build?            [y/n] : y

sudo scripts/build-openfhe-development.sh
```

3. Build the library.

```
mkdir build
cd build
mkdir data
cmake .. -DCMAKE_PREFIX_PATH=~/openfhe-configurator/openfhe-staging/install # adjust the path to the location of the openfhe libraries
make
```

## To Run

- Make sure that the directory `/build/data` exists.
- To run with predefined parameters:
```
# ./test <OMR/OMD> <number_of_payloads> <number_of_pertinent_payloads>
./test OMR 65536 50
./test OMD 65536 50
./test OMR 524288 50
./test OMD 524288 50
```

- To run with custom parameters (Recommended only if you are sufficiently knowledgeable, as custom parameters may result in incorrect, insecure, or inefficient outcomes.):
```
# Customize the parameters in global.h. (Preset to parameters for "./test OMR 65536 50")
./test
```
