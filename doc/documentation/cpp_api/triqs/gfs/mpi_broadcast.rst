..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__mpi_broadcast:

triqs::gfs::mpi_broadcast
=========================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block_gf\<V, T\> <triqs__gfs__block_gf>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    2. | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block_gf_view\<V, T\> <triqs__gfs__block_gf_view>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    3. | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block_gf_const_view\<V, T\> <triqs__gfs__block_gf_const_view>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    4. | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block2_gf\<V, T\> <triqs__gfs__block2_gf>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    5. | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block2_gf_view\<V, T\> <triqs__gfs__block2_gf_view>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    6. | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block2_gf_const_view\<V, T\> <triqs__gfs__block2_gf_const_view>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

Documentation



 **1)**   Initiate (lazy) MPI Bcast

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Bcast operation is performed.



 **2)**   Initiate (lazy) MPI Bcast

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Bcast operation is performed.



 **3)**   Initiate (lazy) MPI Bcast

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Bcast operation is performed.



 **4)**   Initiate (lazy) MPI Bcast

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Bcast operation is performed.



 **5)**   Initiate (lazy) MPI Bcast

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Bcast operation is performed.



 **6)**   Initiate (lazy) MPI Bcast

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Bcast operation is performed.





Parameters
^^^^^^^^^^

 * :param:`g` The Green function

 * :param:`c` The MPI communicator (default is world)

 * :param:`root` The root of the broadcast communication in the MPI sense.


Returns
^^^^^^^

Returns a lazy object describing the object and the MPI operation to be performed.