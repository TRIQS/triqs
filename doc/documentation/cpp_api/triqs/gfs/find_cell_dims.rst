..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__find_cell_dims:

triqs::gfs::find_cell_dims
==========================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | utility::mini_vector<int, 3> :red:`find_cell_dims` (arrays::matrix<double> const & :param:`inv_n`)





Compute dimensions of a parallelepiped cluster cell using the inverse of the periodization matrix





Parameters
^^^^^^^^^^

 * :param:`inv_n` inverse :math:`N^{-1}` of the periodization matrix


Returns
^^^^^^^

the dimensions of the parallelepiped unit cell

   for a given Bravais lattice (defined by unit vectors :math:`{a_i}_{i=0\dots d-1}`), the periodic boundary conditions are uniquely
   defined by the matrix :math:`N` such that the super vectors :math:`\tilde{a}_i` are given by:



.. math::
		\tilde{a}_i = \sum_j N_{ij} a_j

..


   This function computes the dimensions of a parallelepipedic super unit cell (i.e corresponding to the super vectors).

   Example:
    If :math:`N_{ij}` is diag{n_k1, n_k2, n_k3}, this returns {n_k1, n_k2, n_k3}

   The algorithm used is the following:
   let :math:`C={(0,0,0)}`
   for each dimension :math:`d=1\dots 3` :
     - Find the lowest nonzero integer :math:`k_d` such that there exists :math:`mathbf{x}` in C such for all :math:`d_p` in :math:`1\dots 3`, :math:`k_d mathbf{a}_d - mathbf{x}` belongs to the superlattice.
     - Update :math:`C = {mathbf{x} + q mathbf{a}_d, mathbf{x}\in C, q=0\dots k_d-1}`
   return {k_d}_{k=1\dots d}