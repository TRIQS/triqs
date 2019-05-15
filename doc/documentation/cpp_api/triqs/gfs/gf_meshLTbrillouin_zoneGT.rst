..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__gf_meshLTbrillouin_zoneGT:

triqs::gfs::gf_mesh<brillouin_zone>
===================================

Defined in header <**>

.. code-block:: c

     class gf_mesh<brillouin_zone>

Mesh on Brillouin zone




Public members
--------------

+----------------------+------------------------------+-----------------------------+
| units                | matrix<double>               |                             |
+----------------------+------------------------------+-----------------------------+
| periodization_matrix | matrix<int>                  |                             |
+----------------------+------------------------------+-----------------------------+
| dims                 | utility::mini_vector<int, 3> |  the size in each dimension |
+----------------------+------------------------------+-----------------------------+
| _size                | size_t                       |                             |
+----------------------+------------------------------+-----------------------------+
| s1                   | long                         |                             |
+----------------------+------------------------------+-----------------------------+
| s2                   | long                         |                             |
+----------------------+------------------------------+-----------------------------+


Member types
------------

+-------------+--------------------------------+-------------------------------------------------------------+
| var_t       | triqs::lattice::brillouin_zone |                                                             |
+-------------+--------------------------------+-------------------------------------------------------------+
| domain_t    | triqs::lattice::brillouin_zone |  ----------- Model the mesh concept  ---------------------- |
+-------------+--------------------------------+-------------------------------------------------------------+
| domain_pt_t | typename domain_t::point_t     |                                                             |
+-------------+--------------------------------+-------------------------------------------------------------+


Member functions
----------------

+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`constructor <gf_meshLTbrillouin_zoneGT_constructor>`                       |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`_modulo <gf_meshLTbrillouin_zoneGT__modulo>`                               |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`rank <gf_meshLTbrillouin_zoneGT_rank>`                                     |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`get_dimensions <gf_meshLTbrillouin_zoneGT_get_dimensions>`                 |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`domain <gf_meshLTbrillouin_zoneGT_domain>`                                 |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`size <gf_meshLTbrillouin_zoneGT_size>`                                     |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`size_of_components <gf_meshLTbrillouin_zoneGT_size_of_components>`         |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`index_to_point <gf_meshLTbrillouin_zoneGT_index_to_point>`                 | from the index (n_i) to the cartesian coordinates |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`index_to_linear <gf_meshLTbrillouin_zoneGT_index_to_linear>`               | flatten the index                                 |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`is_within_boundary <gf_meshLTbrillouin_zoneGT_is_within_boundary>`         | Is the point in the mesh ? Always true            |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`operator[] <gf_meshLTbrillouin_zoneGT_operator[]>`                         | Accessing a point of the mesh from its index      |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`begin <gf_meshLTbrillouin_zoneGT_begin>`                                   |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`end <gf_meshLTbrillouin_zoneGT_end>`                                       |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`cbegin <gf_meshLTbrillouin_zoneGT_cbegin>`                                 |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`cend <gf_meshLTbrillouin_zoneGT_cend>`                                     |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`operator== <gf_meshLTbrillouin_zoneGT_operator==>`                         | Mesh comparison                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`operator!= <gf_meshLTbrillouin_zoneGT_operator!=>`                         |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`modulo_reduce <gf_meshLTbrillouin_zoneGT_modulo_reduce>`                   | Reduce point modulo to the lattice.               |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`locate_neighbours <gf_meshLTbrillouin_zoneGT_locate_neighbours>`           | locate the closest point                          |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`serialize <gf_meshLTbrillouin_zoneGT_serialize>`                           |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`get_interpolation_data <gf_meshLTbrillouin_zoneGT_get_interpolation_data>` | FIXME : INCORRECT                                 |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`evaluate <gf_meshLTbrillouin_zoneGT_evaluate>`                             |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`index_modulo <gf_meshLTbrillouin_zoneGT_index_modulo>`                     | Reduce index modulo to the lattice.               |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`hdf5_scheme <gf_meshLTbrillouin_zoneGT_hdf5_scheme>`                       | -------------- HDF5  --------------------------   |
+----------------------------------------------------------------------------------+---------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTbrillouin_zoneGT/constructor
    gf_meshLTbrillouin_zoneGT/_modulo
    gf_meshLTbrillouin_zoneGT/rank
    gf_meshLTbrillouin_zoneGT/get_dimensions
    gf_meshLTbrillouin_zoneGT/domain
    gf_meshLTbrillouin_zoneGT/size
    gf_meshLTbrillouin_zoneGT/size_of_components
    gf_meshLTbrillouin_zoneGT/index_to_point
    gf_meshLTbrillouin_zoneGT/index_to_linear
    gf_meshLTbrillouin_zoneGT/is_within_boundary
    gf_meshLTbrillouin_zoneGT/operator[]
    gf_meshLTbrillouin_zoneGT/begin
    gf_meshLTbrillouin_zoneGT/end
    gf_meshLTbrillouin_zoneGT/cbegin
    gf_meshLTbrillouin_zoneGT/cend
    gf_meshLTbrillouin_zoneGT/operator==
    gf_meshLTbrillouin_zoneGT/operator!=
    gf_meshLTbrillouin_zoneGT/modulo_reduce
    gf_meshLTbrillouin_zoneGT/locate_neighbours
    gf_meshLTbrillouin_zoneGT/serialize
    gf_meshLTbrillouin_zoneGT/get_interpolation_data
    gf_meshLTbrillouin_zoneGT/evaluate
    gf_meshLTbrillouin_zoneGT/index_modulo
    gf_meshLTbrillouin_zoneGT/hdf5_scheme


Non Member functions
--------------------

+--------------------------------------------------------------+------------------------------------------------+
| :ref:`operator\<\< <gf_meshLTbrillouin_zoneGT_operatorLTLT>` | -------------------- print ------------------- |
+--------------------------------------------------------------+------------------------------------------------+
| :ref:`boost <gf_meshLTbrillouin_zoneGT_boost>`               |                                                |
+--------------------------------------------------------------+------------------------------------------------+
| :ref:`h5_write <gf_meshLTbrillouin_zoneGT_h5_write>`         |                                                |
+--------------------------------------------------------------+------------------------------------------------+
| :ref:`h5_read <gf_meshLTbrillouin_zoneGT_h5_read>`           |                                                |
+--------------------------------------------------------------+------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTbrillouin_zoneGT/operator<<
    gf_meshLTbrillouin_zoneGT/boost
    gf_meshLTbrillouin_zoneGT/h5_write
    gf_meshLTbrillouin_zoneGT/h5_read