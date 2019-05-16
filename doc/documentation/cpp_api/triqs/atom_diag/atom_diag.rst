..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atom_diag:

triqs::atom_diag::atom_diag
===========================

*#include <triqs/atom_diag.hpp>*

.. rst-class:: cppsynopsis

     template<bool Complex> class  :red:`atom_diag`

Forward declarations


Member types
------------

+----------------------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------+
| scalar_t                   | std::conditional_t<Complex, std::complex<double>, double> |  Type of matrix elements (double/dcomplex)                                            |
+----------------------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------+
| matrix_t                   | matrix<atom_diag::scalar_t>                               |  Matrix type                                                                          |
+----------------------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------+
| block_matrix_t             | std::vector<matrix_t>                                     |  Block-diagonal matrix type                                                           |
+----------------------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------+
| full_hilbert_space_state_t | vector<atom_diag::scalar_t>                               |  State vector in the full Hilbert space, written in the eigenbasis of :math:`\hat H`. |
+----------------------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------+
| many_body_op_t             | triqs::operators::many_body_operator_generic<scalar_t>    |  Many-body operator type used by this atom_diag specialization.                       |
+----------------------------+-----------------------------------------------------------+---------------------------------------------------------------------------------------+


Member functions
----------------

+---------------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__atom_diag__atom_diag__constructor>`                             |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_h_atomic <triqs__atom_diag__atom_diag__get_h_atomic>`                             |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_fops <triqs__atom_diag__atom_diag__get_fops>`                                     |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_full_hilbert_space <triqs__atom_diag__atom_diag__get_full_hilbert_space>`         |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_full_hilbert_space_dim <triqs__atom_diag__atom_diag__get_full_hilbert_space_dim>` |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`n_subspaces <triqs__atom_diag__atom_diag__n_subspaces>`                               |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_subspace_dim <triqs__atom_diag__atom_diag__get_subspace_dim>`                     |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_fock_states <triqs__atom_diag__atom_diag__get_fock_states>`                       |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_unitary_matrices <triqs__atom_diag__atom_diag__get_unitary_matrices>`             |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`flatten_subspace_index <triqs__atom_diag__atom_diag__flatten_subspace_index>`         |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`index_range_of_subspace <triqs__atom_diag__atom_diag__index_range_of_subspace>`       |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_eigensystems <triqs__atom_diag__atom_diag__get_eigensystems>`                     |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_eigenvalue <triqs__atom_diag__atom_diag__get_eigenvalue>`                         |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_energies <triqs__atom_diag__atom_diag__get_energies>`                             |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_quantum_numbers <triqs__atom_diag__atom_diag__get_quantum_numbers>`               |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_gs_energy <triqs__atom_diag__atom_diag__get_gs_energy>`                           |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_vacuum_subspace_index <triqs__atom_diag__atom_diag__get_vacuum_subspace_index>`   |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_vacuum_state <triqs__atom_diag__atom_diag__get_vacuum_state>`                     |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`c_connection <triqs__atom_diag__atom_diag__c_connection>`                             |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`cdag_connection <triqs__atom_diag__atom_diag__cdag_connection>`                       |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`c_matrix <triqs__atom_diag__atom_diag__c_matrix>`                                     |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`cdag_matrix <triqs__atom_diag__atom_diag__cdag_matrix>`                               |  |
+---------------------------------------------------------------------------------------------+--+
| :ref:`get_op_mat <triqs__atom_diag__atom_diag__get_op_mat>`                                 |  |
+---------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    atom_diag/constructor
    atom_diag/get_h_atomic
    atom_diag/get_fops
    atom_diag/get_full_hilbert_space
    atom_diag/get_full_hilbert_space_dim
    atom_diag/n_subspaces
    atom_diag/get_subspace_dim
    atom_diag/get_fock_states
    atom_diag/get_unitary_matrices
    atom_diag/flatten_subspace_index
    atom_diag/index_range_of_subspace
    atom_diag/get_eigensystems
    atom_diag/get_eigenvalue
    atom_diag/get_energies
    atom_diag/get_quantum_numbers
    atom_diag/get_gs_energy
    atom_diag/get_vacuum_subspace_index
    atom_diag/get_vacuum_state
    atom_diag/c_connection
    atom_diag/cdag_connection
    atom_diag/c_matrix
    atom_diag/cdag_matrix
    atom_diag/get_op_mat


Non Member functions
--------------------

+-----------------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__atom_diag__atom_diag__operatorLTLT>` |  |
+-----------------------------------------------------------------+--+

.. toctree::
    :hidden:

    atom_diag/operator<<