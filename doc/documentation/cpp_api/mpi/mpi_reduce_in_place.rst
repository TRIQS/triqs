..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mpi_reduce_in_place:

mpi_reduce_in_place
===================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`---------------- reduce in place  ---------------------`
       | :green:`template<typename T>`
       | void :red:`mpi_reduce_in_place` (std::vector<T> & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)