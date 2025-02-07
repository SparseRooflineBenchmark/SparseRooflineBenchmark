# The Sparse Roofline Benchmark Suite

The Sparse Roofline Benchmark Suite (SRBS) is a collection of sparse kernels
that are used to evaluate the performance of sparse matrix kernels on modern
architectures.

## Installation

The SRBS uses Julia to generate datasets. To install Julia, please follow
the instructions on the [Julia website](https://julialang.org/downloads/).

An example reference C++ implementation is provided in the `example/` directory.
All of the necessary files required to read a test dataset and benchmark the
implementation are provided as header-only dependencies. The only requirement is
a C++20 compliant compiler.

## Usage

Once Julia is installed, you can generate test data for a particular problem
with the `src/Generator/generator.jl` script:

```bash
% julia src/Generator/generator.jl --help
```

The script will generate a dataset for a particular problem and write it to
the given destination. 

For example, to generate RMAT data for spmv in the default location under
`./data/` run the following command: 
```bash
% julia src/Generator/generator.jl spmv RMAT
```

Implementations of a given kernel are expected to be given as executables that
take the same arguments as the reference implementation. All of the
implementations take the same arguments, namely:

```bash
% ./spmv --help
Usage: ./spmv [OPTIONS]
Options:
  -h, --help      Print this help message
  -i, --input     Specify the path for the inputs
  -o, --output    Specify the path for the outputs
  -v, --verbose   Print verbose output
```

The above example implementation can be generated from `examples/spmv.cpp`. 
An example command to generate the executable using g++ is:
```bash
cd examples/
g++ -std=c++20 -o spmv spmv.cpp
```

Harnesses are provided to parse these arguments in several languages. To
benchmark your implementation in C++, simply include `src/benchmark.hpp`. This file includes the necessary header-only
dependencies to parse the arguments and read the input data, and calls the
`experiment` function, which is expected to benchmark the kernel.

## Formats

The SRBS uses the
[Binsparse](https://github.com/GraphBLAS/binsparse-specification) format to
store tensors on disk and in memory. In particular, the header of the file is a
JSON object that describes the dimensions and format of the tensor, and the data
for the tensor is stored as a set of named vectors. These objects may all be
stored in the same file, (e.g. [HDF5](https://www.hdfgroup.org/solutions/hdf5/)
or [ZARR](https://zarr.readthedocs.io/en/stable/)), or in separate files (e.g.
[.npy](https://numpy.org/doc/stable/reference/generated/numpy.load.html)).
