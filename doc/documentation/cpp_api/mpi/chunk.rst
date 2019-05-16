..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _mpi__chunk:

mpi::chunk
==========

*#include <mpi.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | auto :red:`chunk` (T && :param:`range`, :ref:`communicator <mpi__communicator>` :param:`comm` = {})





Function to chunk a range, distributing it uniformly over all MPI ranks.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`T` The type of the range


Parameters
^^^^^^^^^^

 * :param:`range` The range to chunk

 * :param:`comm` The mpi communicator