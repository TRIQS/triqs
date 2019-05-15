..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _atomic_density_matrix:

atomic_density_matrix
=====================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`The atomic density matrix`
       | :green:`template<bool Complex>`
       | typename atom_diag<Complex>::block_matrix_t :red:`atomic_density_matrix` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |   double :param:`beta`)







Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.


Returns
^^^^^^^

 * Gibbs' density matrix of the system.