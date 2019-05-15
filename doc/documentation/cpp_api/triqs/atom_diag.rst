Reference C++ API for triqs::atom_diag
######################################



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

+------------------------------------------------+----------------------+
| :ref:`atom_diag <triqs__atom_diag__atom_diag>` | Forward declarations |
+------------------------------------------------+----------------------+

.. toctree::
    :hidden:

    atom_diag/atom_diag


Functions
---------

+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`partition_function <partition_function>`                                 | The atomic partition function                                                                      |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`atomic_density_matrix <atomic_density_matrix>`                           | The atomic density matrix                                                                          |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`trace_rho_op <trace_rho_op>`                                             | Compute Tr (op * density_matrix)                                                                   |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`act <act>`                                                               | Act with operator `op` on state `st`                                                               |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`quantum_number_eigenvalues <quantum_number_eigenvalues>`                 | Compute values of a given quantum number for all eigenstates                                       |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`quantum_number_eigenvalues_checked <quantum_number_eigenvalues_checked>` | Compute values of a given quantum number for all eigenstates                                       |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`atomic_g_lehmann <atomic_g_lehmann>`                                     | The atomic Green's function, Lehmann representation                                                |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`atomic_g_tau <atomic_g_tau>`                                             | The atomic imaginary time Green's function, constructed from precomputed Lehmann representation    |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`atomic_g_iw <atomic_g_iw>`                                               | The atomic Matsubara Green's function, constructed from precomputed Lehmann representation         |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`atomic_g_l <atomic_g_l>`                                                 | The atomic Green's function in Legendre basis, constructed from precomputed Lehmann representation |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+
| :ref:`atomic_g_w <atomic_g_w>`                                                 | The atomic retarded Green's function, constructed from precomputed Lehmann representation          |
+--------------------------------------------------------------------------------+----------------------------------------------------------------------------------------------------+

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