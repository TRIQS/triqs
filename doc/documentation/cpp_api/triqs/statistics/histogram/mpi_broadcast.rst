..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _histogram_mpi_broadcast:

triqs::statistics::histogram::mpi_broadcast
===========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`MPI-broadcast histogram`
       | void :red:`mpi_broadcast` (:ref:`histogram <triqs__statistics__histogram>` & :param:`h`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)







Parameters
^^^^^^^^^^

 * **h**: Histogram to be broadcast

 * **c**: MPI communicator object

 * **root**: MPI root rank for broadcast operation