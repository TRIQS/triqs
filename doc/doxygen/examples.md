@page examples Examples

[TOC]

This page collects compilable C++ examples that exercise the TRIQS library. 

For the equivalent Python usage, see the Python user guide.

| Example | Description |
|---------|-------------|
| @ref mesh_ex | C++ mesh types — constructing each mesh and iterating its points |
| @ref gf_ex | Green's functions — creating and manipulating @f$G(i\omega)@f$, @f$G(\tau)@f$, @f$G(t)@f$, ... |

@section compiling Compiling the examples

All examples have been compiled on a MacBook Pro with an Apple M2 Max chip with
- [HDF5](https://www.hdfgroup.org/solutions/hdf5/) 2.1.1,
- [open-mpi](https://www.open-mpi.org/) 5.0.1,
- [OpenBLAS](https://www.openblas.net/) 0.3.33,
- [FFTW](https://www.fftw.org/) 3.3.10,
- [GMP](https://gmplib.org/) 6.3.0 and
- [Boost](https://www.boost.org/) 1.89.0

installed via [homebrew](https://brew.sh/).

We further used clang 22.1.4 together with cmake 4.3.2.

Assuming that **TRIQS** has been installed locally (see the
[installation instructions](https://triqs.github.io/triqs/latest/install.html))
and that the actual example code is in a file `main.cpp`, the following generic
`CMakeLists.txt` should work for all examples (see also @ref integration):

```cmake
cmake_minimum_required(VERSION 3.22)
project(example CXX)

# set required standard
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find TRIQS
find_package(triqs REQUIRED CONFIG)

# build the example
add_executable(ex main.cpp)
target_link_libraries(ex triqs)
```
