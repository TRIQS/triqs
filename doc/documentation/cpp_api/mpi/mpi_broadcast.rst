..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _mpi__mpi_broadcast:

mpi::mpi_broadcast
==================

*#include <mpi.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename T>`
       | std::enable_if_t<is_basic<T>::value, void> :red:`mpi_broadcast` (T & :param:`a`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    2. | :green:`template<typename T>`
       | void :red:`mpi_broadcast` (std::vector<T> & :param:`v`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    3. | void :red:`mpi_broadcast` (std::string & :param:`s`, :ref:`communicator <mpi__communicator>` :param:`c`, int :param:`root`)

Documentation



 **1)**   NOTE: We keep the naming mpi_XXX for the actual implementation functions
 // so they can be defined in other namespaces and the mpi::reduce(T,...) function
 // can find them via ADL



 **2)**   ---------------- broadcast ---------------------



 **3)**   ---------------- broadcast ---------------------