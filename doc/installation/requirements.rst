.. index:: Required libraries

.. _requirements:

A complete list of requirements
===============================

TRIQS is built upon several tools, C++ libraries and Python packages, which, 
if not present already in your system, can be freely downloaded and installed. 
All the dependencies of TRIQS are listed below with a brief description.

.. _require_cxx_compilers:

C++ compilers
-------------

TRIQS is written in modern C++, i.e. using the latest `C++ standard <https://isocpp.org/std/status>`_ as defined by the ISO standardization committee.
It is our philsophy to follow the C++ language evolution closely in order to keep the TRIQS code-base as simple and
maintainable as possible. For this reason we have decided to limit the compilation of TRIQS to the rapidly evolving gcc and LLVM based compilers, including clang and IntelLLVM.
We support the following compiler versions:

* g++ 12.x and higher

* :ref:`clang 15.0<install_clang>` and higher

* IntelLLVM (icx) 2023.1.0 and higher

Tools and Libraries
-------------------

+------------------------+-----------+-----------------------------------------------------------------------------+
| Tool/Library           | Version   | Comment                                                                     |
+========================+===========+=============================================================================+
| cmake                  | >= 3.20   | CMake is used to control the software compilation process                   |
+------------------------+-----------+-----------------------------------------------------------------------------+
| python                 | >= 3.6    | The Python interpreter                                                      |
+------------------------+-----------+-----------------------------------------------------------------------------+
| mpi                    | openmpi   | Parallelism                                                                 |
+------------------------+-----------+-----------------------------------------------------------------------------+
| hdf5                   | >= 1.8.2  | File storage system                                                         |
+------------------------+-----------+-----------------------------------------------------------------------------+
| fftw                   | >= 3.2    | Fourier transforms                                                          |
+------------------------+-----------+-----------------------------------------------------------------------------+
| boost                  | >= 1.53   | C++ library collection. We only depend on the boost headers                 |
+------------------------+-----------+-----------------------------------------------------------------------------+
| GMP                    | >= ?      | GNU Multiple Precision Arithmetic Library                                   |
+------------------------+-----------+-----------------------------------------------------------------------------+
| numpy                  | >= 1.11.0 | Python Package - Array library                                              |
+------------------------+-----------+-----------------------------------------------------------------------------+
| OpenMP                 | >= 4      | Parallelism                                                                 |
+------------------------+-----------+-----------------------------------------------------------------------------+
| scipy                  | >= ?      | Python Package - Scientific computing library                               |
+------------------------+-----------+-----------------------------------------------------------------------------+
| matplotlib             | >= 0.99   | Python Package - Plotting library                                           |
+------------------------+-----------+-----------------------------------------------------------------------------+
| mpi4py                 | >= ?      | Python Package - MPI bindings                                               |
+------------------------+-----------+-----------------------------------------------------------------------------+
| mako                   | >= 0.9.1  | Python Package - Template library                                           |
+------------------------+-----------+-----------------------------------------------------------------------------+

Building the TRIQS documentation
--------------------------------

If you want to build the documentation of the TRIQS library locally on your machine, additional dependencies are required.

+------------------------+----------+-----------------------------------------------------------------------------+
| Library/tool           | Version  | Comment                                                                     |
+========================+==========+=============================================================================+
| sphinx                 | >= 1.0.1 | Python Package - Documentation tools                                        |
+------------------------+----------+-----------------------------------------------------------------------------+
| numpydoc               | >= ?     | Sphinx extension to support docstrings in Numpy format                      |
+------------------------+----------+-----------------------------------------------------------------------------+
| libclang               | 15.0     | C interface to clang                                                        |
+------------------------+----------+-----------------------------------------------------------------------------+
| clang                  | 15.0     | Python Package - Libclang bindings                                          |
+------------------------+----------+-----------------------------------------------------------------------------+
