..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atomic_density_matrix:

triqs::atom_diag::atomic_density_matrix
=======================================

*#include <triqs/atom_diag/functions.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<bool Complex>`
       | typename atom_diag<Complex>::block_matrix_t :red:`atomic_density_matrix` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |                        double :param:`beta`)





The atomic density matrix





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Complex` Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * :param:`atom` Solved diagonalization problem.

 * :param:`beta` Inverse temperature.


Returns
^^^^^^^

Gibbs' density matrix of the system.