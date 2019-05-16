..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__act:

triqs::atom_diag::act
=====================

*#include <triqs/atom_diag/functions.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<bool Complex>`
       | typename atom_diag<Complex>::full_hilbert_space_state_t :red:`act` (typename atom_diag<Complex>::many_body_op_t const & :param:`op`,
       |      typename atom_diag<Complex>::full_hilbert_space_state_t const & :param:`st`,
       |      :ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`)





Act with operator `op` on state `st`





Parameters
^^^^^^^^^^

 * :param:`op` Operator to act on the state.

 * :param:`st` Initial state vector in the full Hilbert space, written in the eigenbasis of the Hamiltonian.

 * :param:`atom` Solved diagonalization problem.


Returns
^^^^^^^

Final state vector in the full Hilbert space.