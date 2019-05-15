..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_meshLTcyclic_latticeGT_index_to_point:

triqs::gfs::gf_mesh::index_to_point
===================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`from the index (n_i) to the cartesian coordinates`
       | cluster_mesh::point_t :red:`index_to_point` (cluster_mesh::index_t const & :param:`n`) const





   for a point M of coordinates n_i in the {a_i} basis, the cartesian coordinates are


.. math::
		 OM_i = \sum_j n_j X_{ji}

..





Parameters
^^^^^^^^^^

 * **index_t**: the (integer) coordinates of the point (in basis a_i)

.. warning::
    can be made faster by writing this a matrix-vector multiplication