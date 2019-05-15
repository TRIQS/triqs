..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_meshLTimfreqGT_constructor:

triqs::gfs::gf_mesh::constructor
================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`default constructor`
       | :red:`gf_mesh` ()

    2. | :cppbrief:`constructor`
       | :red:`gf_mesh` (gf_mesh<imfreq>::domain_t :param:`dom`,
       |   long :param:`n_pts` = 1025,
       |   matsubara_mesh_opt :param:`opt` = matsubara_mesh_opt::all_frequencies)

    3. | :red:`gf_mesh` (double :param:`beta`,
       |   statistic_enum :param:`S`,
       |   long :param:`n_pts` = 1025,
       |   matsubara_mesh_opt :param:`opt` = matsubara_mesh_opt::all_frequencies)

    4. | :cppbrief:`constructor`
       | :red:`gf_mesh` (gf_mesh<imfreq>::domain_t :param:`dom`,
       |   energy_t :param:`omega_max`,
       |   matsubara_mesh_opt :param:`opt` = matsubara_mesh_opt::all_frequencies)

    5. | :cppbrief:`constructor`
       | :red:`gf_mesh` (double :param:`beta`,
       |   statistic_enum :param:`S`,
       |   energy_t :param:`omega_max`,
       |   matsubara_mesh_opt :param:`opt` = matsubara_mesh_opt::all_frequencies)

Documentation



 **2)**
 Full constructor





Parameters
^^^^^^^^^^

 * **dom**: domain

 * **n_pts**: defined as n_pts = n_max + 1 (n_max: last matsubara index)

 * **tail_fraction**: : percent of the function in the tail.

 * **tail_order**: : order of computation of the tail

 * **matsubara_mesh_opt**: tells whether the mesh is defined for all frequencies or only for positive frequencies

 * **beta**: inverse temperature

 * **S**: statistic (Fermion or Boson)