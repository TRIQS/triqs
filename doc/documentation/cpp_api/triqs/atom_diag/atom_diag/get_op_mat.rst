..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _atom_diag_get_op_mat:

triqs::atom_diag::atom_diag::get_op_mat
=======================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Get block matrix representation for general operator`
       | atom_diag::op_block_mat_t :red:`get_op_mat` (atom_diag::many_body_op_t const & :param:`op`) const







Parameters
^^^^^^^^^^

 * **op**: Many body operator


Returns
^^^^^^^

 * The block matrix representation of the operator (in the Hamiltonian eigen basis)

 Throws, in case the provided operator does not respect the block symmetries used in the diagonalization.