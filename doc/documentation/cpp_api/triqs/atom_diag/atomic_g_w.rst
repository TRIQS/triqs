..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atomic_g_w:

triqs::atom_diag::atomic_g_w
============================

*#include <triqs/atom_diag/gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<bool Complex>`
       | block_gf<refreq> :red:`atomic_g_w` (gf_lehmann_t<Complex> const & :param:`lehmann`,
       |             gf_struct_t const & :param:`gf_struct`,
       |             gf_mesh<refreq> const & :param:`mesh`,
       |             double :param:`broadening` = 0)

    2. | :green:`template<bool Complex>`
       | block_gf<refreq> :red:`atomic_g_w` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |             double :param:`beta`,
       |             gf_struct_t const & :param:`gf_struct`,
       |             std::pair<double, double> const & :param:`energy_window`,
       |             int :param:`n_w`,
       |             double :param:`broadening` = 0,
       |             excluded_states_t const & :param:`excluded_states` = {})

Documentation



 **1)**   The atomic retarded Green's function, constructed from precomputed Lehmann representation



 **2)**   The atomic retarded Green's function, possibly with excluded states (none by default)





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Complex` Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * :param:`lehmann` Lehmann representation.

 * :param:`gf_struct` Block structure of the Green's function, block name -> list of inner indices.

 * :param:`mesh` Real frequency mesh used in construction.

 * :param:`broadening` Lorentian broadening of the spectrum (imaginary frequency shift).

 * :param:`atom` Solved diagonalization problem.

 * :param:`beta` Inverse temperature.

 * :param:`energy_window` Energy window :math:`(\omega_{min}, \omega_{max})`.

 * :param:`n_w` Number of frequency points.

 * :param:`excluded_states` Excluded eigenstates as pairs (subspace index, inner index).


Returns
^^^^^^^

Atomic Green's function :math:`G_{at}(\omega)`.