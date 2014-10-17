.. highlight:: c

MPI and serialization  
##########################################

Serialization 
============================

The `value classes` and the views are compatible with Boost Serialization library.

MPI
============================

The `value classes` (array, matrix, vector) can be bcasted, reduced, 
with the boost.mpi library, e.g. :

.. triqs_example:: ./IO_0.cpp
