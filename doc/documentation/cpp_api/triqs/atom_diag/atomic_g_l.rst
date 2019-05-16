..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atomic_g_l:

triqs::atom_diag::atomic_g_l
============================

*#include <triqs/atom_diag/gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<bool Complex>`
       | block_gf<legendre> :red:`atomic_g_l` (gf_lehmann_t<Complex> const & :param:`lehmann`,
       |             gf_struct_t const & :param:`gf_struct`,
       |             :ref:`gf_mesh\<legendre\> <triqs__gfs__gf_meshLTlegendreGT>` const & :param:`mesh`)

    2. | :green:`template<bool Complex>`
       | block_gf<legendre> :red:`atomic_g_l` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`,
       |             double :param:`beta`,
       |             gf_struct_t const & :param:`gf_struct`,
       |             int :param:`n_l`,
       |             excluded_states_t const & :param:`excluded_states` = {})

Documentation



 **1)**   The atomic Green's function in Legendre basis, constructed from precomputed Lehmann representation



 **2)**   The atomic Green's function in Legendre basis, possibly with excluded states (none by default)





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Complex` Are we using Lehmann representation with complex matrix elements?


Parameters
^^^^^^^^^^

 * :param:`lehmann` Lehmann representation.

 * :param:`gf_struct` Block structure of the Green's function, block name -> list of inner indices.

 * :param:`mesh` Legendre mesh used in construction.

 * :param:`atom` Solved diagonalization problem.

 * :param:`beta` Inverse temperature.

 * :param:`n_l` Number of Legendre coefficients.

 * :param:`excluded_states` Excluded eigenstates as pairs (subspace index, inner index).


Returns
^^^^^^^

Atomic Green's function :math:`G_{at}(\ell)`.