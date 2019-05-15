..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mpi_reduce:

mpi_reduce
==========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Initiate (lazy) MPI Reduce`
       | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block_gf\<V, T\> <triqs__gfs__block_gf>` const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)

    2. | :cppbrief:`Initiate (lazy) MPI Reduce`
       | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block_gf_view\<V, T\> <triqs__gfs__block_gf_view>` const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)

    3. | :cppbrief:`Initiate (lazy) MPI Reduce`
       | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block_gf_const_view\<V, T\> <triqs__gfs__block_gf_const_view>` const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)

    4. | :cppbrief:`Initiate (lazy) MPI Reduce`
       | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block2_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block2_gf\<V, T\> <triqs__gfs__block2_gf>` const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)

    5. | :cppbrief:`Initiate (lazy) MPI Reduce`
       | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block2_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block2_gf_view\<V, T\> <triqs__gfs__block2_gf_view>` const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)

    6. | :cppbrief:`Initiate (lazy) MPI Reduce`
       | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block2_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block2_gf_const_view\<V, T\> <triqs__gfs__block2_gf_const_view>` const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)

Documentation



 **1)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Reduce operation is performed.



 **2)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Reduce operation is performed.



 **3)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Reduce operation is performed.



 **4)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Reduce operation is performed.



 **5)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Reduce operation is performed.



 **6)**
 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Reduce operation is performed.





Parameters
^^^^^^^^^^

 * **g**: The Green function

 * **c**: The MPI communicator (default is world)

 * **root**: The root of the broadcast communication in the MPI sense.


Returns
^^^^^^^

 * Returns a lazy object describing the object and the MPI operation to be performed.