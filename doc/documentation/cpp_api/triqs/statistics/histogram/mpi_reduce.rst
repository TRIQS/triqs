..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__statistics__histogram__mpi_reduce:

triqs::statistics::histogram::mpi_reduce
========================================

*#include <triqs/statistics.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :ref:`histogram <triqs__statistics__histogram>` :red:`mpi_reduce` (:ref:`histogram <triqs__statistics__histogram>` const & :param:`h`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false,
       |             MPI_Op :param:`op` = MPI_SUM)





MPI-reduce histogram

   The only supported reduction operation is MPI_SUM, which is equivalent to `operator+()`.





Parameters
^^^^^^^^^^

 * :param:`h` Histogram subject to reduction

 * :param:`c` MPI communicator object

 * :param:`root` MPI root rank for MPI reduction

 * :param:`all` Send reduction result to all ranks in `c`?

 * :param:`op` Reduction operation, must be MPI_SUM


Returns
^^^^^^^

Reduction result; valid only on MPI rank 0 if `all = false`