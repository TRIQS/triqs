..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _atomic_g_w:

atomic_g_w
==========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`The atomic retarded Green's function, constructed from precomputed Lehmann representation`
       | :green:`template<bool Complex>`
       | block_gf<refreq> :red:`atomic_g_w` (gf_lehmann_t<Complex> const & :param:`lehmann`,
       |   gf_struct_t const & :param:`gf_struct`,
       |   gf_mesh<refreq> const & :param:`mesh`,
       |   double :param:`broadening` = 0)

    2. | :cppbrief:`The atomic retarded Green's function, possibly with excluded states (none by default)`
       | :green:`template<bool Complex>`
       | block_gf<refreq> :red:`atomic_g_w` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |   double :param:`beta`,
       |   gf_struct_t const & :param:`gf_struct`,
       |   std::pair<double, double> const & :param:`energy_window`,
       |   int :param:`n_w`,
       |   double :param:`broadening` = 0,
       |   excluded_states_t const & :param:`excluded_states` = {})

Documentation





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * **lehmann**: Lehmann representation.

 * **gf_struct**: Block structure of the Green's function, block name -> list of inner indices.

 * **mesh**: Real frequency mesh used in construction.

 * **broadening**: Lorentian broadening of the spectrum (imaginary frequency shift).

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.

 * **energy_window**: Energy window :math:`(\omega_{min}, \omega_{max})`.

 * **n_w**: Number of frequency points.

 * **excluded_states**: Excluded eigenstates as pairs (subspace index, inner index).


Returns
^^^^^^^

 * Atomic Green's function :math:`G_{at}(\omega)`.