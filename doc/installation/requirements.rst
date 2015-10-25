.. index:: Required libraries

.. _requirements:

A complete list of requirements
===============================

TRIQS is built upon several python and C++ libraries, which, if not present
already in your system, can be freely downloaded and installed. All the
libraries and tools used by TRIQS are described below.

.. _require_cxx_compilers:

C++ compilers
-------------

TRIQS is written in C++, i.e. in the **C++** standard as defined by the ISO standardization committee, 
i.e. currently C++14.
A recent compiler is therefore mandatory.

* Standard compliant C++ compilers (C++14).

  * g++ 4.9.1 and higher.
  
  * :ref:`clang 3.4<install_clang>` and higher (in particular the default clang on OS X >= 10.8).
    The recommandation is to use the latest version of clang available.

  * Intel icc 16.0 seems to be fine too (it implements C++14).
    The intel icl on OS X (with the clang front end) seems to be working as well.
    In both cases, tests are still needed with these compilers, since none of the developers
    use them on a routine basis. 

* In C++11 backward compatibility mode only:
  This mode is **deprecated**.
  
  It is sufficient for a basic usage of the library, but 
  it excludes several of its new features.
  
  C++11 mode **will be removed** in future releases in 2016, in order
  to simplify the codes and their maintenance.

  So if you using C++11, you are invited to upgrade your compiler.

  * g++ 4.8.2 and higher (there seems to be a bug in 4.8.1).

* C++98 compilers are obsolete and will *never* be supported.

  * g++ before 4.8.1
  * Intel icc 15.0 and below


Libraries
---------

+------------------------+----------+------------------------------------------------------------------------+
| Library/tool           | Version  | Comment                                                                |
+========================+==========+========================================================================+
| mpi                    | openmpi  | Parallelism (1).                                                       |
+------------------------+----------+------------------------------------------------------------------------+
| fftw                   | >= 3.2   | Fourier transforms                                                     |
+------------------------+----------+------------------------------------------------------------------------+
| boost                  | >= 1.49  | C++ librairies                                                         |
+------------------------+----------+------------------------------------------------------------------------+
| hdf5                   | >= 1.8.2 | File storage system. Important: the *serial* version must be installed |
+------------------------+----------+------------------------------------------------------------------------+
| python                 | >= 2.7   | The Python interpreter                                                 |
+------------------------+----------+------------------------------------------------------------------------+
| scipy                  | >= ?     | Python mathematical library                                            |
+------------------------+----------+------------------------------------------------------------------------+
| numpy                  | >= ?     | Python scientific library                                              |
+------------------------+----------+------------------------------------------------------------------------+
| h5py                   | >= ?     | Python interface to hdf5 library                                       |
+------------------------+----------+------------------------------------------------------------------------+
| matplotlib             | >= 0.99  | Python 2D plotting library                                             |
+------------------------+----------+------------------------------------------------------------------------+
| mpi4py                 | >= ?     | Python MPI                                                             |
+------------------------+----------+------------------------------------------------------------------------+
| cmake                  | >= 2.8.7 | CMake is used to control the software compilation process              |
+------------------------+----------+------------------------------------------------------------------------+
| mako                   | >= 0.9.1 | mako templates are used to generate the C++/python wrapper             |
+------------------------+----------+------------------------------------------------------------------------+
| sphinx                 | >= 1.0.1 | Python documentation tools (to compile documentation, for developers)  |
+------------------------+----------+------------------------------------------------------------------------+
| pyparsing              | >= ?     | Tool for sphinx (to compile documentation, for developers)             |
+------------------------+----------+------------------------------------------------------------------------+
| libclang               | 3.4      | python bindings of the clang lib (for developers)                      |
+------------------------+----------+------------------------------------------------------------------------+

(1)  Since standard linux distributions (and macports on OS X) now provides openmpi, even on laptops, we avoid the unnecessary complication of maintaining a non-parallel version of TRIQS

