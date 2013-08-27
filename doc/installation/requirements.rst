.. index:: required libraries

.. _requirements:

A complete list of requirements
================================

TRIQS is built upon several python and C++ libraries, which, if not present
already in your system, can be freely downloaded and installed. All the
libraries and tools used by TRIQS are listed in the table:

.. _require_cxx_compilers:


C++ compilers
-----------------

TRIQS is written in C++, i.e. in the **C++11** standard as defined by the ISO.
A recent compiler is therefore mandatory.
There are 3 categories of C++ compilers.

* Standard compliant C++ compilers (recommended and supported).
  
   * :ref:`clang 3.3<install_clang>` and higher. 
   * g++ 4.8.1 and higher

* Obsolete compilers.

   TRIQS compiles on them, but they lack full C++11 support. 
   Therefore TRIQS may be slower, since some optimisations are disabled in the library
   and they may not be supported in later release.
 
   * g++ 4.6.4 to 4.8.0

* C++98 compilers.

  Compilers implementing previous version of the C++ standard (c++98) do not work, and 
  will *never* be supported.  This includes : 

   * g++ before 4.6
   * Intel icc 12.0 and below.


Libraries
---------------

==================    ================  ================================================================================
Libraries/tools       Version           Comment
==================    ================  ================================================================================
mpi                   openmpi           Parallelism
                                        Since standard linux distributions (and macports on OS X)
                                        now provides openmpi, even on laptops, we avoid the unnecessary complication
                                        of maintaining a non parallel version of TRIQS
fftw                  >= 3.2            Fourier transform
boost                 >= 1.49           C++ librairies
hdf5                  >= 1.8.0          File storage system. Important: the *serial* version must be installed
python                >= 2.6.5
scipy                                   python mathematical library
numpy                                   python scientific library
h5py                                    python interface to hdf5 library
mpi4py                                  python MPI 
sphinx                >= 1.0.1          python documentation tools (to compile documentation)
pyparsing                               Tool for sphinx (to compile documentation)
matplotlib            >= 0.99           python 2D plotting library
cython                >=0.17            cython package
==================    ================  ================================================================================


