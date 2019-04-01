..
   Generated automatically by cpp2rst

.. highlight:: c


.. _atomic_g_tau:

atomic_g_tau
============

**Synopsis**:

.. code-block:: c

    template<bool Complex>                                                                                                                                                                                                                 (1)
    block_gf<triqs::gfs::imtime> atomic_g_tau (const gf_lehmann_t<Complex> & lehmann,
   const triqs::hilbert_space::gf_struct_t & gf_struct, const
   gf_mesh<triqs::gfs::imtime> & mesh)

    template<bool Complex>                                                                                                                                                                                                                 (2)
    block_gf<triqs::gfs::imtime> atomic_g_tau (const atom_diag<Complex> & atom, double
   beta, const triqs::hilbert_space::gf_struct_t & gf_struct, int n_tau, const
   triqs::atom_diag::excluded_states_t & excluded_states = {})

(1)The atomic imaginary time Green's function, constructed from precomputed Lehmann representation


Template parameters
-------------------

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?



Parameters
----------

 * **lehmann**: Lehmann representation.

 * **gf_struct**: Block structure of the Green's function, block name -> list of inner indices.

 * **mesh**: Imaginary time mesh used in construction.



Return value
------------

Atomic Green's function :math:`G_{at}(\tau)`


Documentation
-------------




(2)The atomic imaginary time Green's function, possibly with excluded states (none by default)


Template parameters
-------------------

 * **Complex**: Are we using Lehmann representation with complex matrix elements?



Parameters
----------

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.

 * **gf_struct**: Block structure of the Green's function, block name -> list of inner indices.

 * **n_tau**: Number of imaginary time points.

 * **excluded_states**: Excluded eigenstates as pairs (subspace index, inner index).



Return value
------------

Atomic Green's function :math:`G_{at}(\tau)`


Documentation
-------------