..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mpi_gather:

mpi_gather
==========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`---------------- gather  ---------------------`
       | :green:`template<typename T>`
       | std::vector<T> :red:`mpi_gather` (std::vector<T> const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false)