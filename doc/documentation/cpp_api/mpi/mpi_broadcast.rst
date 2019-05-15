..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mpi_broadcast:

mpi_broadcast
=============


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`NOTE: We keep the naming mpi_XXX for the actual implementation functions`
       | :green:`template<typename T>`
       | std::enable_if_t<is_basic<T>::value, void> :red:`mpi_broadcast` (T & :param:`a`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    2. | :cppbrief:`---------------- broadcast ---------------------`
       | :green:`template<typename T>`
       | void :red:`mpi_broadcast` (std::vector<T> & :param:`v`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    3. | :cppbrief:`---------------- broadcast ---------------------`
       | void :red:`mpi_broadcast` (std::string & :param:`s`, :ref:`communicator <mpi__communicator>` :param:`c`, int :param:`root`)

Documentation



 **1)**      so they can be defined in other namespaces and the mpi::reduce(T,...) function
   can find them via ADL