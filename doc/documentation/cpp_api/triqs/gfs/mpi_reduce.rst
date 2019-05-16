..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__mpi_reduce:

triqs::gfs::mpi_reduce
======================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block_gf\<V, T\> <triqs__gfs__block_gf>` const & :param:`a`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false,
       |             MPI_Op :param:`op` = MPI_SUM)

    2. | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block_gf_view\<V, T\> <triqs__gfs__block_gf_view>` const & :param:`a`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false,
       |             MPI_Op :param:`op` = MPI_SUM)

    3. | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block_gf_const_view\<V, T\> <triqs__gfs__block_gf_const_view>` const & :param:`a`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false,
       |             MPI_Op :param:`op` = MPI_SUM)

    4. | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block2_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block2_gf\<V, T\> <triqs__gfs__block2_gf>` const & :param:`a`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false,
       |             MPI_Op :param:`op` = MPI_SUM)

    5. | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block2_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block2_gf_view\<V, T\> <triqs__gfs__block2_gf_view>` const & :param:`a`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false,
       |             MPI_Op :param:`op` = MPI_SUM)

    6. | :green:`template<typename V, typename T>`
       | :ref:`mpi_lazy\<tag::reduce, block2_gf_const_view\<V, T\> \> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (:ref:`block2_gf_const_view\<V, T\> <triqs__gfs__block2_gf_const_view>` const & :param:`a`,
       |             :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |             int :param:`root` = 0,
       |             bool :param:`all` = false,
       |             MPI_Op :param:`op` = MPI_SUM)

Documentation



 **1)**   Initiate (lazy) MPI Reduce

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Reduce operation is performed.



 **2)**   Initiate (lazy) MPI Reduce

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Reduce operation is performed.



 **3)**   Initiate (lazy) MPI Reduce

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Reduce operation is performed.



 **4)**   Initiate (lazy) MPI Reduce

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Reduce operation is performed.



 **5)**   Initiate (lazy) MPI Reduce

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Reduce operation is performed.



 **6)**   Initiate (lazy) MPI Reduce

When the returned object is used at the RHS of operator = or in a constructor of a gf,
the MPI Reduce operation is performed.





Parameters
^^^^^^^^^^

 * :param:`g` The Green function

 * :param:`c` The MPI communicator (default is world)

 * :param:`root` The root of the broadcast communication in the MPI sense.


Returns
^^^^^^^

Returns a lazy object describing the object and the MPI operation to be performed.