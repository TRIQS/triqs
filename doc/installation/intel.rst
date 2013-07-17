.. index:: icc

.. _icc:

.. highlight:: bash

Using Intel icc compiler
==========================

While less user friendly than clang, Intel compilers are usually recommended on some machines
because of optimisation.

Version
---------

The current version of TRIQS compiles only on Intel C++ version 13.0 
(because of the lack of C++11 support in previous versions).

NB: previous releases (<= v0.9 compiles on icc >= 11).

Usage 
-------------

To compile TRIQS, you will need : 

* a new and patched version of boost 1.51 (because of a stupid C++11 issue in one file...).

  PROVIDE IT WITH THE LINK ...

* to pass the option to the compiler ::

     -DBOOST_MATH_DISABLE_STD_FPCLASSIFY

  e.g. in configuring:: 

     CXX=icc CXXFLAGS="-DBOOST_MATH_DISABLE_STD_FPCLASSIFY" cmake path_to_triqs_sources ......

  .. warning:: Do it or tests will fail and computations will be WRONG

  To be written : link to the ticket.

Caveats
--------------

* icc does not provide its own implementation of the standard library
  (given the price, it is quite disappointing).
  So it uses gcc's standard lib.
  It must use gcc 4.6 or 4.7.

* Linking with mkl is always a mess....

  TO BE WRITTEN ...


