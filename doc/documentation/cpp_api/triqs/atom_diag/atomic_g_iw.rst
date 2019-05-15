..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _atomic_g_iw:

atomic_g_iw
===========


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`The atomic Matsubara Green's function, constructed from precomputed Lehmann representation`
       | :green:`template<bool Complex>`
       | block_gf<imfreq> :red:`atomic_g_iw` (gf_lehmann_t<Complex> const & :param:`lehmann`,
       |   gf_struct_t const & :param:`gf_struct`,
       |   :ref:`gf_mesh\<imfreq\> <triqs__gfs__gf_meshLTimfreqGT>` const & :param:`mesh`)

    2. | :cppbrief:`The atomic Matsubara Green's function, possibly with excluded states (none by default)`
       | :green:`template<bool Complex>`
       | block_gf<imfreq> :red:`atomic_g_iw` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |   double :param:`beta`,
       |   gf_struct_t const & :param:`gf_struct`,
       |   int :param:`n_iw`,
       |   excluded_states_t const & :param:`excluded_states` = {})

Documentation





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Complex**: Are we using Lehmann representation with complex matrix elements?


Parameters
^^^^^^^^^^

 * **lehmann**: Lehmann representation.

 * **gf_struct**: Block structure of the Green's function, block name -> list of inner indices.

 * **mesh**: Matsubara mesh used in construction.

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.

 * **n_iw**: Number of Matsubara frequencies.

 * **excluded_states**: Excluded eigenstates as pairs (subspace index, inner index).


Returns
^^^^^^^

 * Atomic Green's function :math:`G_{at}(i\omega)`.