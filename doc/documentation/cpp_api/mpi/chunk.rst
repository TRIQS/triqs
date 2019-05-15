..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _chunk:

chunk
=====


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Function to chunk a range, distributing it uniformly over all MPI ranks.`
       | :green:`template<typename T>`
       | auto :red:`chunk` (T && :param:`range`, :ref:`communicator <mpi__communicator>` :param:`comm` = {})







Template parameters
^^^^^^^^^^^^^^^^^^^

 * **T**: The type of the range


Parameters
^^^^^^^^^^

 * **range**: The range to chunk

 * **comm**: The mpi communicator