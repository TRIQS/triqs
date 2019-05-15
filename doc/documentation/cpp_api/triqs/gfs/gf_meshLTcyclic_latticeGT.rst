..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__gf_meshLTcyclic_latticeGT:

triqs::gfs::gf_mesh<cyclic_lattice>
===================================

Defined in header <**>

.. code-block:: c

     class gf_mesh<cyclic_lattice>

Mesh on real-space lattice




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

+----------+---------------------------------+--+
| var_t    | cyclic_lattice                  |  |
+----------+---------------------------------+--+
| domain_t | triqs::lattice::bravais_lattice |  |
+----------+---------------------------------+--+


Member functions
----------------

+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`constructor <gf_meshLTcyclic_latticeGT_constructor>`                       | full constructor                                  |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`_modulo <gf_meshLTcyclic_latticeGT__modulo>`                               |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`rank <gf_meshLTcyclic_latticeGT_rank>`                                     |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`get_dimensions <gf_meshLTcyclic_latticeGT_get_dimensions>`                 |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`domain <gf_meshLTcyclic_latticeGT_domain>`                                 |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`size <gf_meshLTcyclic_latticeGT_size>`                                     |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`size_of_components <gf_meshLTcyclic_latticeGT_size_of_components>`         |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`index_to_point <gf_meshLTcyclic_latticeGT_index_to_point>`                 | from the index (n_i) to the cartesian coordinates |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`index_to_linear <gf_meshLTcyclic_latticeGT_index_to_linear>`               | flatten the index                                 |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`is_within_boundary <gf_meshLTcyclic_latticeGT_is_within_boundary>`         | Is the point in the mesh ? Always true            |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`operator[] <gf_meshLTcyclic_latticeGT_operator[]>`                         | Accessing a point of the mesh from its index      |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`begin <gf_meshLTcyclic_latticeGT_begin>`                                   |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`end <gf_meshLTcyclic_latticeGT_end>`                                       |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`cbegin <gf_meshLTcyclic_latticeGT_cbegin>`                                 |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`cend <gf_meshLTcyclic_latticeGT_cend>`                                     |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`operator== <gf_meshLTcyclic_latticeGT_operator==>`                         | Mesh comparison                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`operator!= <gf_meshLTcyclic_latticeGT_operator!=>`                         |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`modulo_reduce <gf_meshLTcyclic_latticeGT_modulo_reduce>`                   | Reduce point modulo to the lattice.               |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`locate_neighbours <gf_meshLTcyclic_latticeGT_locate_neighbours>`           | locate the closest point                          |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`serialize <gf_meshLTcyclic_latticeGT_serialize>`                           |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`index_modulo <gf_meshLTcyclic_latticeGT_index_modulo>`                     | Reduce index modulo to the lattice.               |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`get_interpolation_data <gf_meshLTcyclic_latticeGT_get_interpolation_data>` |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`evaluate <gf_meshLTcyclic_latticeGT_evaluate>`                             |                                                   |
+----------------------------------------------------------------------------------+---------------------------------------------------+
| :ref:`hdf5_scheme <gf_meshLTcyclic_latticeGT_hdf5_scheme>`                       | -------------- HDF5  --------------------------   |
+----------------------------------------------------------------------------------+---------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTcyclic_latticeGT/constructor
    gf_meshLTcyclic_latticeGT/_modulo
    gf_meshLTcyclic_latticeGT/rank
    gf_meshLTcyclic_latticeGT/get_dimensions
    gf_meshLTcyclic_latticeGT/domain
    gf_meshLTcyclic_latticeGT/size
    gf_meshLTcyclic_latticeGT/size_of_components
    gf_meshLTcyclic_latticeGT/index_to_point
    gf_meshLTcyclic_latticeGT/index_to_linear
    gf_meshLTcyclic_latticeGT/is_within_boundary
    gf_meshLTcyclic_latticeGT/operator[]
    gf_meshLTcyclic_latticeGT/begin
    gf_meshLTcyclic_latticeGT/end
    gf_meshLTcyclic_latticeGT/cbegin
    gf_meshLTcyclic_latticeGT/cend
    gf_meshLTcyclic_latticeGT/operator==
    gf_meshLTcyclic_latticeGT/operator!=
    gf_meshLTcyclic_latticeGT/modulo_reduce
    gf_meshLTcyclic_latticeGT/locate_neighbours
    gf_meshLTcyclic_latticeGT/serialize
    gf_meshLTcyclic_latticeGT/index_modulo
    gf_meshLTcyclic_latticeGT/get_interpolation_data
    gf_meshLTcyclic_latticeGT/evaluate
    gf_meshLTcyclic_latticeGT/hdf5_scheme


Non Member functions
--------------------

+--------------------------------------------------------------+------------------------------------------------+
| :ref:`operator\<\< <gf_meshLTcyclic_latticeGT_operatorLTLT>` | -------------------- print ------------------- |
+--------------------------------------------------------------+------------------------------------------------+
| :ref:`h5_write <gf_meshLTcyclic_latticeGT_h5_write>`         |                                                |
+--------------------------------------------------------------+------------------------------------------------+
| :ref:`h5_read <gf_meshLTcyclic_latticeGT_h5_read>`           |                                                |
+--------------------------------------------------------------+------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTcyclic_latticeGT/operator<<
    gf_meshLTcyclic_latticeGT/h5_write
    gf_meshLTcyclic_latticeGT/h5_read