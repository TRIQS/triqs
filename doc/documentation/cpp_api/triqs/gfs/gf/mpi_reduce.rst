..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_mpi_reduce:

triqs::gfs::gf::mpi_reduce
==========================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Initiate (lazy) MPI Reduce`
       | :ref:`mpi_lazy\<tag::reduce, gf::const_view_type\> <triqs__gfs__mpi_lazy>` :red:`mpi_reduce` (gf<Var, Target> const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false,
       |   MPI_Op :param:`op` = MPI_SUM)






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