..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__quantum_number_eigenvalues:

triqs::atom_diag::quantum_number_eigenvalues
============================================

*#include <triqs/atom_diag/functions.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<bool Complex>`
       | std::vector<std::vector<quantum_number_t> > :red:`quantum_number_eigenvalues` (typename atom_diag<Complex>::many_body_op_t const & :param:`op`,
       |                             :ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`)





Compute values of a given quantum number for all eigenstates





Parameters
^^^^^^^^^^

 * :param:`op` Observable operator; supposed to be a quantum number (if not -> exception).

 * :param:`atom` Solved diagonalization problem.


Returns
^^^^^^^

The eigenvalues by block