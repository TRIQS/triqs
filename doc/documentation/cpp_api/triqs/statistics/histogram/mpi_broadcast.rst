..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__statistics__histogram__mpi_broadcast:

triqs::statistics::histogram::mpi_broadcast
===========================================

*#include <triqs/statistics.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`mpi_broadcast` (:ref:`histogram <triqs__statistics__histogram>` & :param:`h`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)





MPI-broadcast histogram





Parameters
^^^^^^^^^^

 * :param:`h` Histogram to be broadcast

 * :param:`c` MPI communicator object

 * :param:`root` MPI root rank for broadcast operation