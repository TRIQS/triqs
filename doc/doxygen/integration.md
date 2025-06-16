@page integration Integration in C++ projects

[TOC]

**TRIQS** is a rather big library that can take a considerable amount of time to compile.
We therefore recommend to build and install **TRIQS** beforehand (see
[Installation](https://triqs.github.io/triqs/latest/install.html)) and then link against it (see @ref find_package).

The following assumes that the project using **TRIQS** is CMake-based.

@section cmake CMake

@subsection find_package find_package

If you have already installed **TRIQS** on your system by following the instructions from the
[Installation](https://triqs.github.io/triqs/latest/install.html) page, you can make use of CMake's
[find_package](https://cmake.org/cmake/help/latest/command/find_package.html) command.

Let's assume that **TRIQS** has been installed to `path_to_install_dir`.
Then linking your project to **TRIQS** with CMake is as easy as

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_project CXX)

# find TRIQS
find_package(triqs REQUIRED CONFIG)

# declare a target and link to TRIQS
add_executable(my_executable main.cpp)
target_link_libraries(my_executable triqs)
```

In case, CMake cannot find the package, you might have to tell it where to look for the `TRIQSConfig.cmake` file by
setting the variable `TRIQS_DIR` to `path_to_install_dir/lib/cmake/triqs` or by sourcing the provided `triqsvars.sh`
before running CMake:

```console
$ source path_to_install_dir/share/triqs/triqsvars.sh
```
