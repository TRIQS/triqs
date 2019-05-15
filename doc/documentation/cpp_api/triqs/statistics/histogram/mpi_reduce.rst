..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _histogram_mpi_reduce:

triqs::statistics::histogram::mpi_reduce
========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`MPI-reduce histogram`
       | :ref:`histogram <triqs__statistics__histogram>` :red:`mpi_reduce` (:ref:`histogram <triqs__statistics__histogram>` const & :param:`h`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)






    The only supported reduction operation is MPI_SUM, which is equivalent to `operator+()`.





Parameters
^^^^^^^^^^

 * **h**: Histogram subject to reduction

 * **c**: MPI communicator object

 * **root**: MPI root rank for MPI reduction

 * **all**: Send reduction result to all ranks in `c`?

 * **op**: Reduction operation, must be MPI_SUM


Returns
^^^^^^^

 * Reduction result; valid only on MPI rank 0 if `all = false`