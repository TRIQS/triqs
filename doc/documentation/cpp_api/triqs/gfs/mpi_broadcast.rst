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

    1. | :cppbrief:`Initiate (lazy) MPI Bcast`
       | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block_gf\<V, T\> <triqs__gfs__block_gf>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    2. | :cppbrief:`Initiate (lazy) MPI Bcast`
       | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block_gf_view\<V, T\> <triqs__gfs__block_gf_view>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    3. | :cppbrief:`Initiate (lazy) MPI Bcast`
       | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block_gf_const_view\<V, T\> <triqs__gfs__block_gf_const_view>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    4. | :cppbrief:`Initiate (lazy) MPI Bcast`
       | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block2_gf\<V, T\> <triqs__gfs__block2_gf>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    5. | :cppbrief:`Initiate (lazy) MPI Bcast`
       | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block2_gf_view\<V, T\> <triqs__gfs__block2_gf_view>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

    6. | :cppbrief:`Initiate (lazy) MPI Bcast`
       | :green:`template<typename V, typename T>`
       | void :red:`mpi_broadcast` (:ref:`block2_gf_const_view\<V, T\> <triqs__gfs__block2_gf_const_view>` & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)

Documentation



 **1)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Bcast operation is performed.



 **2)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Bcast operation is performed.



 **3)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Bcast operation is performed.



 **4)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Bcast operation is performed.



 **5)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Bcast operation is performed.



 **6)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Bcast operation is performed.





Parameters
^^^^^^^^^^

 * **g**: The Green function

 * **c**: The MPI communicator (default is world)

 * **root**: The root of the broadcast communication in the MPI sense.


Returns
^^^^^^^

 * Returns a lazy object describing the object and the MPI operation to be performed.