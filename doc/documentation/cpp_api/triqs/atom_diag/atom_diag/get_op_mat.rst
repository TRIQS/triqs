..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atom_diag__get_op_mat:

triqs::atom_diag::atom_diag::get_op_mat
=======================================

*#include <triqs/atom_diag/atom_diag.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | atom_diag::op_block_mat_t :red:`get_op_mat` (atom_diag::many_body_op_t const & :param:`op`) const





Get block matrix representation for general operator





Parameters
^^^^^^^^^^

 * :param:`op` Many body operator


Returns
^^^^^^^

The block matrix representation of the operator (in the Hamiltonian eigen basis)

 Throws, in case the provided operator does not respect the block symmetries used in the diagonalization.