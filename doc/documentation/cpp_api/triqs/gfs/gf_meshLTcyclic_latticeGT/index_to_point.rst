..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTcyclic_latticeGT__index_to_point:

triqs::gfs::gf_mesh<cyclic_lattice>::index_to_point
===================================================

*#include <triqs/gfs/cluster_mesh.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | cluster_mesh::point_t :red:`index_to_point` (cluster_mesh::index_t const & :param:`n`) const





 from the index (n_i) to the cartesian coordinates
 for a point M of coordinates n_i in the {a_i} basis, the cartesian coordinates are


.. math::
		 OM_i = \sum_j n_j X_{ji}

..





Parameters
^^^^^^^^^^

 * :param:`index_t` the (integer) coordinates of the point (in basis a_i)


.. warning::
    can be made faster by writing this a matrix-vector multiplication