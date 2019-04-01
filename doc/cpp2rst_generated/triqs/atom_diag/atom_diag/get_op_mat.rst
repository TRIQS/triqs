..
   Generated automatically by cpp2rst

.. highlight:: c


.. _atom_diag_get_op_mat:

get_op_mat
==========

**Synopsis**:

.. code-block:: c

    triqs::atom_diag::atom_diag::op_block_mat_t get_op_mat (const
   triqs::atom_diag::atom_diag::many_body_op_t & op)

Get block matrix representation for general operator


Parameters
----------

 * **op**: Many body operator



Return value
------------

The block matrix representation of the operator (in the Hamiltonian eigen basis)

 Throws, in case the provided operator does not respect the block symmetries used in the diagonalization.

Documentation
-------------