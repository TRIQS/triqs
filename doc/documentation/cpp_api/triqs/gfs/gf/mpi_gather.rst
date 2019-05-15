..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_mpi_gather:

triqs::gfs::gf::mpi_gather
==========================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Initiate (lazy) MPI Gather`
       | :ref:`mpi_lazy\<tag::gather, gf::const_view_type\> <triqs__gfs__mpi_lazy>` :red:`mpi_gather` (gf<Var, Target> const & :param:`a`,
       |   :ref:`communicator <mpi__communicator>` :param:`c` = {},
       |   int :param:`root` = 0,
       |   bool :param:`all` = false)






 When the returned object is used at the RHS of operator = or in a constructor of a gf,
 the MPI Gather operation is performed.





Parameters
^^^^^^^^^^

 * **g**: The Green function

 * **c**: The MPI communicator (default is world)

 * **root**: The root of the broadcast communication in the MPI sense.


Returns
^^^^^^^

 * Returns a lazy object describing the object and the MPI operation to be performed.