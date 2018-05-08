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

TRIQS is written in modern C++, i.e. using the latest **C++** standard as defined by the ISO standardization committee.
In particular, the latest TRIQS version uses language features from the C++17 standard. 
A recent compiler is therefore mandatory.

* Standard compliant C++ compilers (C++17).

  * g++ 7.0.0 and higher.

  * :ref:`clang 5.0<install_clang>` and higher.

  .. note:: Intel's compiler icc has **not** yet fully implemented the C++17 standard.
            It can therefore not be used to compile TRIQS.

Tools and Libraries
-------------------

+------------------------+----------+-----------------------------------------------------------------------------+
| Tool/Library           | Version  | Comment                                                                     |
+========================+==========+=============================================================================+
| cmake                  | >= 2.8.7 | CMake is used to control the software compilation process                   |
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
