..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atomic_g_tau:

triqs::atom_diag::atomic_g_tau
==============================

*#include <triqs/atom_diag/gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<bool Complex>`
       | block_gf<imtime> :red:`atomic_g_tau` (gf_lehmann_t<Complex> const & :param:`lehmann`,
       |               gf_struct_t const & :param:`gf_struct`,
       |               :ref:`gf_mesh\<imtime\> <triqs__gfs__gf_meshLTimtimeGT>` const & :param:`mesh`)

    2. | :green:`template<bool Complex>`
       | block_gf<imtime> :red:`atomic_g_tau` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |               double :param:`beta`,
       |               gf_struct_t const & :param:`gf_struct`,
       |               int :param:`n_tau`,
       |               excluded_states_t const & :param:`excluded_states` = {})

Documentation



 **1)**   The atomic imaginary time Green's function, constructed from precomputed Lehmann representation



 **2)**   The atomic imaginary time Green's function, possibly with excluded states (none by default)





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Complex` Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * :param:`lehmann` Lehmann representation.

 * :param:`gf_struct` Block structure of the Green's function, block name -> list of inner indices.

 * :param:`mesh` Imaginary time mesh used in construction.

 * :param:`atom` Solved diagonalization problem.

 * :param:`beta` Inverse temperature.

 * :param:`n_tau` Number of imaginary time points.

 * :param:`excluded_states` Excluded eigenstates as pairs (subspace index, inner index).


Returns
^^^^^^^

Atomic Green's function :math:`G_{at}(\tau)`