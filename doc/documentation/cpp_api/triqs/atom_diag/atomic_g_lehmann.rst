..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atomic_g_lehmann:

triqs::atom_diag::atomic_g_lehmann
==================================

*#include <triqs/atom_diag/gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<bool Complex>`
       | gf_lehmann_t<Complex> :red:`atomic_g_lehmann` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |                   double :param:`beta`,
       |                   gf_struct_t const & :param:`gf_struct`,
       |                   excluded_states_t :param:`excluded_states` = {})





The atomic Green's function, Lehmann representation





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Complex` Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * :param:`atom` Solved diagonalization problem.

 * :param:`beta` Inverse temperature.

 * :param:`gf_struct` Block structure of the Green's function, block name -> list of inner indices.

 * :param:`excluded_states` Excluded eigenstates as pairs (subspace index, inner index).


Returns
^^^^^^^

Atomic Green's function in the Lehmann representation