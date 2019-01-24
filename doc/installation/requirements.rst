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

TRIQS is written in modern C++, i.e. using the latest `**C++** standard <https://isocpp.org/std/status>`_ as defined by the ISO standardization committee.
It is our philsophy to follow the C++ language evolution closely in order to keep the TRIQS code-base as simple and
maintainable as possible. For this reason we have decided to limit the compilation of TRIQS to the rapidly evolving gcc and clang compilers.
We support the following compiler versions:

* g++ 7.x  **CAUTION**: Due to an on-going `issue <https://github.com/TRIQS/triqs/issues/570>`_ triqs can currently not be compiled with g++ 8.x

* :ref:`clang 5.0<install_clang>` and higher.

Tools and Libraries
-------------------

+------------------------+----------+-----------------------------------------------------------------------------+
| Tool/Library           | Version  | Comment                                                                     |
+========================+==========+=============================================================================+
| cmake                  | >= 2.8.12| CMake is used to control the software compilation process                   |
+------------------------+----------+-----------------------------------------------------------------------------+
| python                 | >= 2.7   | The Python interpreter                                                      |
+------------------------+----------+-----------------------------------------------------------------------------+
| mpi                    | openmpi  | Parallelism (1).                                                            |
+------------------------+----------+-----------------------------------------------------------------------------+
| hdf5                   | >= 1.8.2 | File storage system. Important: the *serial* version must be installed      |
+------------------------+----------+-----------------------------------------------------------------------------+
| fftw                   | >= 3.2   | Fourier transforms                                                          |
+------------------------+----------+-----------------------------------------------------------------------------+
| boost                  | >= 1.49  | C++ library collection                                                      |
+------------------------+----------+-----------------------------------------------------------------------------+
| numpy                  | >= ?     | Python Package - Array library                                              |
+------------------------+----------+-----------------------------------------------------------------------------+
| scipy                  | >= ?     | Python Package - Scientific computing library                               |
+------------------------+----------+-----------------------------------------------------------------------------+
| h5py                   | >= ?     | Python Package - Interface to hdf5 library                                  |
+------------------------+----------+-----------------------------------------------------------------------------+
| matplotlib             | >= 0.99  | Python Package - Plotting library                                           |
+------------------------+----------+-----------------------------------------------------------------------------+
| mpi4py                 | >= ?     | Python Package - MPI bindings                                               | 
+------------------------+----------+-----------------------------------------------------------------------------+
| mako                   | >= 0.9.1 | Python Package - Template library                                           |
+------------------------+----------+-----------------------------------------------------------------------------+

(1)  Since standard linux distributions (and macports on OS X) now provide openmpi, even on laptops, we avoid the
unnecessary complication of maintaining a non-parallel version of TRIQS.

Building the TRIQS documentation
--------------------------------

If you want to build the documentation of the TRIQS library locally on your machine, additional dependencies are required.

+------------------------+----------+-----------------------------------------------------------------------------+
| Library/tool           | Version  | Comment                                                                     |
+========================+==========+=============================================================================+
| libclang               | 5.0      | C interface to clang                                                        |
+------------------------+----------+-----------------------------------------------------------------------------+
| MathJax                | >= ?     | Library for browser Latex rendering                                         |
+------------------------+----------+-----------------------------------------------------------------------------+
| sphinx                 | >= 1.0.1 | Python Package - Documentation tools                                        |
+------------------------+----------+-----------------------------------------------------------------------------+
| clang                  | 5.0      | Python Package - Libclang bindings                                          |
+------------------------+----------+-----------------------------------------------------------------------------+
