..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_meshLTcyclic_latticeGT_constructor:

triqs::gfs::gf_mesh::constructor
================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`full constructor`
       | :red:`gf_mesh` (triqs::lattice::bravais_lattice const & :param:`bl_`, matrix<int> const & :param:`periodization_matrix_`)

    2. | :cppbrief:`Construct gf_mesh<cyclic_lattice> from three linear sizes assuming a cubic lattice (backward compatibility)`
       | :red:`gf_mesh` (int :param:`L1` = 1, int :param:`L2` = 1, int :param:`L3` = 1)

    3. | :cppbrief:`Construct gf_mesh<cyclic_lattice> from domain (bravais_lattice) and int L (linear size of Cluster mesh)`
       | :red:`gf_mesh` (triqs::lattice::bravais_lattice const & :param:`bl_`, int :param:`L`)

Documentation





Parameters
^^^^^^^^^^

 * **bl_**: bravais lattice

 * **periodization_matrix**: such that :math:`\tilde{a}_i = \sum_j N_{ij} a_j`