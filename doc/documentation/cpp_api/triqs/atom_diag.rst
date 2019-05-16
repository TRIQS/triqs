triqs::atom_diag
################



Type aliases
------------

+-------------------+-------------------------------------+---------------------------------------------------------------------------+
| indices_t         | fundamental_operator_set::indices_t |                                                                           |
+-------------------+-------------------------------------+---------------------------------------------------------------------------+
| quantum_number_t  | double                              |  Quantum number operators are Hermitian, hence their eigenvalues are real |
+-------------------+-------------------------------------+---------------------------------------------------------------------------+
| excluded_states_t | std::vector<std::pair<int, int> >   |  List of excluded eigenstates, (subspace index, inner index) pairs        |
+-------------------+-------------------------------------+---------------------------------------------------------------------------+


Classes
-------

.. table::
   :width: 50% 50%

+------------------------------------------------+--+
| :ref:`atom_diag <triqs__atom_diag__atom_diag>` |  |
+------------------------------------------------+--+

.. toctree::
    :hidden:

    atom_diag/atom_diag


Functions
---------

+--------------------------------------------------------------------------------------------------+--+
| :ref:`partition_function <triqs__atom_diag__partition_function>`                                 |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`atomic_density_matrix <triqs__atom_diag__atomic_density_matrix>`                           |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`trace_rho_op <triqs__atom_diag__trace_rho_op>`                                             |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`act <triqs__atom_diag__act>`                                                               |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`quantum_number_eigenvalues <triqs__atom_diag__quantum_number_eigenvalues>`                 |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`quantum_number_eigenvalues_checked <triqs__atom_diag__quantum_number_eigenvalues_checked>` |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`atomic_g_lehmann <triqs__atom_diag__atomic_g_lehmann>`                                     |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`atomic_g_tau <triqs__atom_diag__atomic_g_tau>`                                             |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`atomic_g_iw <triqs__atom_diag__atomic_g_iw>`                                               |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`atomic_g_l <triqs__atom_diag__atomic_g_l>`                                                 |  |
+--------------------------------------------------------------------------------------------------+--+
| :ref:`atomic_g_w <triqs__atom_diag__atomic_g_w>`                                                 |  |
+--------------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    atom_diag/partition_function
    atom_diag/atomic_density_matrix
    atom_diag/trace_rho_op
    atom_diag/act
    atom_diag/quantum_number_eigenvalues
    atom_diag/quantum_number_eigenvalues_checked
    atom_diag/atomic_g_lehmann
    atom_diag/atomic_g_tau
    atom_diag/atomic_g_iw
    atom_diag/atomic_g_l
    atom_diag/atomic_g_w