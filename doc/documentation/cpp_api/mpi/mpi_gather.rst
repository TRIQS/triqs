..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _mpi__mpi_gather:

mpi::mpi_gather
===============

*#include <mpi.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | std::vector<T> :red:`mpi_gather` (std::vector<T> const & :param:`a`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false)





---------------- gather  ---------------------