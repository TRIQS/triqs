..
   Generated automatically by cpp2rst

.. highlight:: c


.. _atomic_g_w:

atomic_g_w
==========

**Synopsis**:

.. code-block:: c

    template<bool Complex>                                                                                                                                                                                                                                                                                         (1)
    block_gf<triqs::gfs::refreq> atomic_g_w (const gf_lehmann_t<Complex> & lehmann, const
   triqs::hilbert_space::gf_struct_t & gf_struct, const gf_mesh<triqs::gfs::refreq> &
   mesh, double broadening = 0)

    template<bool Complex>                                                                                                                                                                                                                                                                                         (2)
    block_gf<triqs::gfs::refreq> atomic_g_w (const atom_diag<Complex> & atom, double beta,
   const triqs::hilbert_space::gf_struct_t & gf_struct, const std::pair<double, double> &
   energy_window, int n_w, double broadening = 0, const
   triqs::atom_diag::excluded_states_t & excluded_states = {})

(1)The atomic retarded Green's function, constructed from precomputed Lehmann representation


Template parameters
-------------------

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?



Parameters
----------

 * **lehmann**: Lehmann representation.

 * **gf_struct**: Block structure of the Green's function, block name -> list of inner indices.

 * **mesh**: Real frequency mesh used in construction.

 * **broadening**: Lorentian broadening of the spectrum (imaginary frequency shift).



Return value
------------

Atomic Green's function :math:`G_{at}(\omega)`.


Documentation
-------------




(2)The atomic retarded Green's function, possibly with excluded states (none by default)


Template parameters
-------------------

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?



Parameters
----------

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.

 * **gf_struct**: Block structure of the Green's function, block name -> list of inner indices.

 * **energy_window**: Energy window :math:`(\omega_{min}, \omega_{max})`.

 * **n_w**: Number of frequency points.

 * **broadening**: Lorentian broadening of the spectrum (imaginary frequency shift).

 * **excluded_states**: Excluded eigenstates as pairs (subspace index, inner index).



Return value
------------

Atomic Green's function :math:`G_{at}(\omega)`.


Documentation
-------------