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
A recent compiler is therefore mandatory.

* Standard compliant C++ compilers (recommended and supported).
  
  * :ref:`clang 3.2<install_clang>` and higher (in particular the default clang on OS X 10.8).
  * g++ 4.8.1 and higher

  * The intel icc 14.0 is close to be C++11 compliant, but presents currently too many bugs to be supported 
    at present. It compiles 99% of TRIQS, but we do not have the ressources to write and 
    maintains all necessary workarounds.
    
    Besides, for *our codes*, icc does not provide a significant speed gain (unlike MKL), 
    so we do not recommend it for TRIQS.

* C++98 compilers

  * g++ before 4.8.1
  * Intel icc 13.0 and below

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
cmake                 >= 2.8.7          CMake is used to control the software compilation process
pyparsing                               Tool for sphinx (to compile documentation)
matplotlib            >= 0.99           Python 2D plotting library
cython                >=0.17            A language that allows to write C extensions for the Python language
==================    ================  ================================================================================


