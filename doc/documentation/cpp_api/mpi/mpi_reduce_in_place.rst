..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _mpi__mpi_reduce_in_place:

mpi::mpi_reduce_in_place
========================

*#include <mpi.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | void :red:`mpi_reduce_in_place` (std::vector<T> & :param:`a`,
       |                      :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |                      int :param:`root` = 0,
       |                      bool :param:`all` = false,
       |                      MPI_Op :param:`op` = MPI_SUM)





---------------- reduce in place  ---------------------