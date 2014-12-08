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

TRIQS is written in C++, i.e. in the **C++11** standard as defined by the ISO standardization committee.
Some advanced parts of the library (multivariable Green functions) require some features of the most recent **C++14** standard.
A recent compiler is therefore mandatory.

* Standard compliant C++ compilers (supported).
  
  * in C++14 mode (recommended):

     * :ref:`clang 3.4<install_clang>` and higher (in particular the default clang on OS X >= 10.8).
     * g++ 4.9.1 and higher.

  * in C++11 mode only:

     * g++ 4.8.2 and higher (there seems to be a bug in 4.8.1).
  
     * The intel icc 15.0 is close to be C++11 compliant, but presents currently too many bugs to be supported 
       at present. It compiles 99% of TRIQS, but we do not have the resources to write and 
       maintains all necessary workarounds.
       The intel icl on OS X (with the clang front end) seems to be working.  

* C++98 compilers do not work and will *not* be supported.

  * g++ before 4.8.1
  * Intel icc 13.0 and below


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
| sphinxcontrib-doxylink | >= ?     | Tool for sphinx (to compile documentation, for developers)             |
+------------------------+----------+------------------------------------------------------------------------+
| libclang               | 3.4      | python bindings of the clang lib (for developers)                      |
+------------------------+----------+------------------------------------------------------------------------+

(1)  Since standard linux distributions (and macports on OS X) now provides openmpi, even on laptops, we avoid the unnecessary complication of maintaining a non-parallel version of TRIQS
 
