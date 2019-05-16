..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _mpi__environment__constructor:

mpi::environment::constructor
=============================

*#include <mpi/environment.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :red:`environment` (int :param:`argc`, char *[] :param:`argv`)





MPICH does not allow Init without argc, argv, so we do not allow default constructors
   // for portability, cf #133