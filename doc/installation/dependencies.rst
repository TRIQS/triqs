.. index:: dependencies

.. _dependencies:

A complete list of dependencies
==================================

TRIQS is built upon several python and C++ libraries, which, if not present already in your system, can be freely downloaded and installed.
All the libraries and tools used by TRIQS are listed in the table : 

==================    ================  ================================================================================
Libraries/tools       Version           Comment
==================    ================  ================================================================================
mpi                   e.g., openmpi     Parallelism
                                    
                                        Since standard linux distributions (and macports on OS X)
                                        now provides openmpi, even on laptops, we avoid the unnecessary complication
                                        of maintaining a non parallel version of TRIQS
fftw                  >= 3.2            Fourier transform
boost                 >= 1.49           C++ librairies
hdf5                  >= 1.8.0          File storage system. Important: the *serial* version must be installed
python*               >= 2.6.5
scipy*                                  python mathematical library
numpy*                                  python scientific library
h5py*                                   python interface to hdf5 library
sphinx*               >= 1.0.1          python documentation tools (to compile documentation)
pyparsing*                              Tool for sphinx (to compile documentation)
matplotlib*           >= 0.99           python 2D plotting library
cython                >=0.17            [DEVELOPERS ONLY]
==================    ================  ================================================================================

 \* designates the libraries included in the Enthought python distribution.

C++11
--------

The current version of TRIQS use the new C++11 standard, and therefore require a recent C++ compiler.

Tested compilers include : 

* g++ 4.6.3, 4.7
* clang++ 3.1
* icc 13.0

Note that older compilers will *never* be supported for current and future version of TRIQS.

Older releases may accept older compilers.

F90
-------
For the Wien2TRIQS interface only.

Tested compilers include : 

* ifort 
* gfortran

Boost 
--------------------------

The boost library is often upgraded, and it is rare to have the correct version in your distribution.  TRIQS installation process offers two choices : 

  * Recommended choice: As explained in the :ref:`page above <installation>`, you can download simply the latest *sources* and TRIQS will do all the job for you by compiling the pieces of boost that are needed in a little boost_for_triqs library.

  * OR you can include and link with an installed boost if the version if high enough as discussed in :ref:`install_options`.



