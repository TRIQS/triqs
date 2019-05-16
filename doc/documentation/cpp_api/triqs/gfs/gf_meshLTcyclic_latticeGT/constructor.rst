..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTcyclic_latticeGT__constructor:

triqs::gfs::gf_mesh<cyclic_lattice>::constructor
================================================

*#include <triqs/gfs/gf_mesh.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`gf_mesh` (triqs::lattice::bravais_lattice const & :param:`bl_`, matrix<int> const & :param:`periodization_matrix_`)

    2. | :red:`gf_mesh` (int :param:`L1` = 1, int :param:`L2` = 1, int :param:`L3` = 1)

    3. | :red:`gf_mesh` (triqs::lattice::bravais_lattice const & :param:`bl_`, int :param:`L`)

Documentation



 **1)**   full constructor



 **2)**   Construct gf_mesh<cyclic_lattice> from three linear sizes assuming a cubic lattice (backward compatibility)



 **3)**   Construct gf_mesh<cyclic_lattice> from domain (bravais_lattice) and int L (linear size of Cluster mesh)





Parameters
^^^^^^^^^^

 * :param:`bl_` bravais lattice

 * :param:`periodization_matrix` such that :math:`\tilde{a}_i = \sum_j N_{ij} a_j`