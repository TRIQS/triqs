..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_mpi_broadcast:

triqs::gfs::gf::mpi_broadcast
=============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Initiate (lazy) MPI Bcast`
       | void :red:`mpi_broadcast` (gf<Var, Target> & :param:`g`, :ref:`communicator <mpi__communicator>` :param:`c` = {}, int :param:`root` = 0)






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