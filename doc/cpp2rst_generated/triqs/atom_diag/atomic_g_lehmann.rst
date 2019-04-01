..
   Generated automatically by cpp2rst

.. highlight:: c


.. _atomic_g_lehmann:

atomic_g_lehmann
================

**Synopsis**:

.. code-block:: c

    template<bool Complex>
    gf_lehmann_t<Complex> atomic_g_lehmann (const atom_diag<Complex> & atom, double beta,
   const triqs::hilbert_space::gf_struct_t & gf_struct,
   triqs::atom_diag::excluded_states_t excluded_states = {})

The atomic Green's function, Lehmann representation


Template parameters
-------------------

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?



Parameters
----------

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.

 * **gf_struct**: Block structure of the Green's function, block name -> list of inner indices.

 * **excluded_states**: Excluded eigenstates as pairs (subspace index, inner index).



Return value
------------

Atomic Green's function in the Lehmann representation


Documentation
-------------