..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_meshLTbrillouin_zoneGT_constructor:

triqs::gfs::gf_mesh::constructor
================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`gf_mesh` ()

    2. | :cppbrief:`full constructor`
       | :red:`gf_mesh` (triqs::lattice::brillouin_zone const & :param:`bz_`, matrix<int> const & :param:`periodization_matrix_`)

    3. | :cppbrief:`backward compatibility`
       | :red:`gf_mesh` (triqs::lattice::brillouin_zone const & :param:`bz_`, int :param:`n_l`)

Documentation



 **3)**    constructs simple bz mesh on square lattice with simple boundary conditions





Parameters
^^^^^^^^^^

 * **bz_**: brillouin zone

 * **periodization_matrix**: such that :math:`\tilde{a}_i = \sum_j N_{ij} a_j`

    Constructs

.. math::
		\tilde{b}_i = \sum_j N^{-1}_{ji} b_j

..
 where :math:`b_j` reciprocal vectors