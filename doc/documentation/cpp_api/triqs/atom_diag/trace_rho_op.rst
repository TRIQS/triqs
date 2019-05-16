..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__trace_rho_op:

triqs::atom_diag::trace_rho_op
==============================

*#include <triqs/atom_diag/functions.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<bool Complex>`
       | typename atom_diag<Complex>::scalar_t :red:`trace_rho_op` (typename atom_diag<Complex>::block_matrix_t const & :param:`density_matrix`,
       |               typename atom_diag<Complex>::many_body_op_t const & :param:`op`,
       |               :ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`)





Compute Tr (op * density_matrix)





Parameters
^^^^^^^^^^

 * :param:`density_matrix` Density matrix as a list of diagonal blocks for all invariant subspaces in `atom`.

 * :param:`op` Operator to be averaged.

 * :param:`atom` Solved diagonalization problem.


Returns
^^^^^^^

Operator `op` averaged over the density matrix.