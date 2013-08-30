.. index:: required libraries

.. _requirements:

A complete list of requirements
===============================

TRIQS is built upon several python and C++ libraries, which, if not present
already in your system, can be freely downloaded and installed. All the
libraries and tools used by TRIQS are described below.

.. _require_cxx_compilers:

C++ compilers
-------------

TRIQS is written in C++, i.e. in the **C++11** standard as defined by the ISO.
A recent compiler is therefore mandatory.
There are 3 categories of C++ compilers.

* Standard compliant C++ compilers
  
  * :ref:`clang 3.3<install_clang>` and higher
  * g++ 4.8.1 and higher

  These compilers are recommended and supported.


* Obsolete compilers

  * g++ 4.6.3 to 4.8.0

  TRIQS can be built with these compilers but because they lack full C++11 support,
  TRIQS may be slower (some optimisations are disabled in the library
  and they may not be supported in later release).

* C++98 compilers

  * g++ before 4.6
  * Intel icc 12.0 and below

  Compilers implementing the previous version of the C++ standard (C++98) do not work and 
  will *not* be supported.



Libraries
---------

==================    ================  ================================================================================
Library/tool          Version           Comment
==================    ================  ================================================================================
mpi                   openmpi           Parallelism.
                                        Since standard linux distributions (and macports on OS X)
                                        now provides openmpi, even on laptops, we avoid the unnecessary complication
                                        of maintaining a non-parallel version of TRIQS
fftw                  >= 3.2            Fourier transforms
boost                 >= 1.49           C++ librairies
hdf5                  >= 1.8.0          File storage system. Important: the *serial* version must be installed
python                >= 2.6.5          The Python interpreter
scipy                                   Python mathematical library
numpy                                   Python scientific library
h5py                                    Python interface to hdf5 library
mpi4py                                  Python MPI 
sphinx                >= 1.0.1          Python documentation tools (to compile documentation)
pyparsing                               Tool for sphinx (to compile documentation)
matplotlib            >= 0.99           Python 2D plotting library
cython                >=0.17            A language that allows to write C extensions for the Python language
==================    ================  ================================================================================


