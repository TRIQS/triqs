..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__atom_diag__atom_diag:

triqs::atom_diag::atom_diag
===========================

Defined in header <**>

.. code-block:: c

    template<bool Complex> class atom_diag

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

+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`constructor <atom_diag_constructor>`                               | Construct in an uninitialized state.                                                        |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_h_atomic <atom_diag_get_h_atomic>`                             | The Hamiltonian used at construction                                                        |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_fops <atom_diag_get_fops>`                                     | The fundamental operator set used at construction                                           |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_full_hilbert_space <atom_diag_get_full_hilbert_space>`         | The full Hilbert space                                                                      |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_full_hilbert_space_dim <atom_diag_get_full_hilbert_space_dim>` | Dimension of the full Hilbert space                                                         |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`n_subspaces <atom_diag_n_subspaces>`                               | Number of invariant subspaces                                                               |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_subspace_dim <atom_diag_get_subspace_dim>`                     | The dimension of a subspace                                                                 |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_fock_states <atom_diag_get_fock_states>`                       | The list of Fock states for each subspace                                                   |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_unitary_matrices <atom_diag_get_unitary_matrices>`             | Unitary matrices that transform from Fock states to eigenstates                             |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`flatten_subspace_index <atom_diag_flatten_subspace_index>`         | Returns the state index in the full Hilbert space given a subspace index and an inner index |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`index_range_of_subspace <atom_diag_index_range_of_subspace>`       | Return the range of indices of subspace sp_index                                            |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_eigensystems <atom_diag_get_eigensystems>`                     | Get the eigensystems for all subspaces                                                      |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_eigenvalue <atom_diag_get_eigenvalue>`                         | Get the i-th eigenvalue of subspace sp_index                                                |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_energies <atom_diag_get_energies>`                             | A vector of all the energies, grouped by subspace                                           |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_quantum_numbers <atom_diag_get_quantum_numbers>`               | A vector of all the quantum numbers, grouped by subspace                                    |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_gs_energy <atom_diag_get_gs_energy>`                           | Ground state energy (i.e. min of all subspaces)                                             |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_vacuum_subspace_index <atom_diag_get_vacuum_subspace_index>`   | Returns invariant subspace containing the vacuum state                                      |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_vacuum_state <atom_diag_get_vacuum_state>`                     | Returns the vacuum state as a vector in the full Hilbert space                              |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`c_connection <atom_diag_c_connection>`                             | Subspace-to-subspace connections for fundamental operator :math:`C`                         |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`cdag_connection <atom_diag_cdag_connection>`                       | Subspace-to-subspace connections for fundamental operator :math:`C^\dagger`                 |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`c_matrix <atom_diag_c_matrix>`                                     | Matrix block for fundamental operator :math:`C`                                             |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`cdag_matrix <atom_diag_cdag_matrix>`                               | Matrix block for fundamental operator :math:`C^\dagger`                                     |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`get_op_mat <atom_diag_get_op_mat>`                                 | Get block matrix representation for general operator                                        |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+
| :ref:`hdf5_scheme <atom_diag_hdf5_scheme>`                               |                                                                                             |
+--------------------------------------------------------------------------+---------------------------------------------------------------------------------------------+

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
    atom_diag/hdf5_scheme


Non Member functions
--------------------

+------------------------------------------------------+--------------------------------------------------------+
| :ref:`atom_diag_worker <atom_diag_atom_diag_worker>` |                                                        |
+------------------------------------------------------+--------------------------------------------------------+
| :ref:`operator\<\< <atom_diag_operatorLTLT>`         | Friend declarations of a template class are a bit ugly |
+------------------------------------------------------+--------------------------------------------------------+
| :ref:`h5_write <atom_diag_h5_write>`                 |                                                        |
+------------------------------------------------------+--------------------------------------------------------+
| :ref:`h5_read <atom_diag_h5_read>`                   |                                                        |
+------------------------------------------------------+--------------------------------------------------------+

.. toctree::
    :hidden:

    atom_diag/atom_diag_worker
    atom_diag/operator<<
    atom_diag/h5_write
    atom_diag/h5_read