..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTimfreqGT__constructor:

triqs::gfs::gf_mesh<imfreq>::constructor
========================================

*#include <triqs/gfs/gf_mesh.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`gf_mesh` ()

    2. | :red:`gf_mesh` (gf_mesh<imfreq>::domain_t :param:`dom`,
       |        long :param:`n_pts` = 1025,
       |        matsubara_mesh_opt :param:`opt` = matsubara_mesh_opt::all_frequencies)

    3. | :red:`gf_mesh` (double :param:`beta`,
       |        statistic_enum :param:`S`,
       |        long :param:`n_pts` = 1025,
       |        matsubara_mesh_opt :param:`opt` = matsubara_mesh_opt::all_frequencies)

    4. | :red:`gf_mesh` (gf_mesh<imfreq>::domain_t :param:`dom`,
       |        energy_t :param:`omega_max`,
       |        matsubara_mesh_opt :param:`opt` = matsubara_mesh_opt::all_frequencies)

    5. | :red:`gf_mesh` (double :param:`beta`,
       |        statistic_enum :param:`S`,
       |        energy_t :param:`omega_max`,
       |        matsubara_mesh_opt :param:`opt` = matsubara_mesh_opt::all_frequencies)

Documentation



 **1)**   default constructor



 **2)**   constructor
 Full constructor



 **4)**   constructor



 **5)**   constructor





Parameters
^^^^^^^^^^

 * :param:`dom` domain

 * :param:`n_pts` defined as n_pts = n_max + 1 (n_max: last matsubara index)

 * :param:`tail_fraction` : percent of the function in the tail.

 * :param:`tail_order` : order of computation of the tail

 * :param:`matsubara_mesh_opt` tells whether the mesh is defined for all frequencies or only for positive frequencies

 * :param:`beta` inverse temperature

 * :param:`S` statistic (Fermion or Boson)