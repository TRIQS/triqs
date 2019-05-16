..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTbrillouin_zoneGT__constructor:

triqs::gfs::gf_mesh<brillouin_zone>::constructor
================================================

*#include <triqs/gfs/gf_mesh.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`gf_mesh` ()

    2. | :red:`gf_mesh` (triqs::lattice::brillouin_zone const & :param:`bz_`, matrix<int> const & :param:`periodization_matrix_`)

    3. | :red:`gf_mesh` (triqs::lattice::brillouin_zone const & :param:`bz_`, int :param:`n_l`)

Documentation



 **2)**   full constructor



 **3)**   backward compatibility
 constructs simple bz mesh on square lattice with simple boundary conditions





Parameters
^^^^^^^^^^

 * :param:`bz_` brillouin zone

 * :param:`periodization_matrix` such that :math:`\tilde{a}_i = \sum_j N_{ij} a_j`

    Constructs

.. math::
		\tilde{b}_i = \sum_j N^{-1}_{ji} b_j

..
 where :math:`b_j` reciprocal vectors