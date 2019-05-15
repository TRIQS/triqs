..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _atomic_g_lehmann:

atomic_g_lehmann
================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`The atomic Green's function, Lehmann representation`
       | :green:`template<bool Complex>`
       | gf_lehmann_t<Complex> :red:`atomic_g_lehmann` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |   double :param:`beta`,
       |   gf_struct_t const & :param:`gf_struct`,
       |   excluded_states_t :param:`excluded_states` = {})







Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.

 * **gf_struct**: Block structure of the Green's function, block name -> list of inner indices.

 * **excluded_states**: Excluded eigenstates as pairs (subspace index, inner index).


Returns
^^^^^^^

 * Atomic Green's function in the Lehmann representation