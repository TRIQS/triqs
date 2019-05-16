..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTbrillouin_zoneGT:

triqs::gfs::gf_mesh<brillouin_zone>
===================================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

      class  :red:`gf_mesh<brillouin_zone>`

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

+-----------------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__gf_meshLTbrillouin_zoneGT__constructor>`                     |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`index_to_point <triqs__gfs__gf_meshLTbrillouin_zoneGT__index_to_point>`                 |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`index_to_linear <triqs__gfs__gf_meshLTbrillouin_zoneGT__index_to_linear>`               |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`is_within_boundary <triqs__gfs__gf_meshLTbrillouin_zoneGT__is_within_boundary>`         |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`operator[] <triqs__gfs__gf_meshLTbrillouin_zoneGT__operator[]>`                         |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`operator== <triqs__gfs__gf_meshLTbrillouin_zoneGT__operator==>`                         |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`modulo_reduce <triqs__gfs__gf_meshLTbrillouin_zoneGT__modulo_reduce>`                   |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`locate_neighbours <triqs__gfs__gf_meshLTbrillouin_zoneGT__locate_neighbours>`           |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`get_interpolation_data <triqs__gfs__gf_meshLTbrillouin_zoneGT__get_interpolation_data>` |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`index_modulo <triqs__gfs__gf_meshLTbrillouin_zoneGT__index_modulo>`                     |  |
+-----------------------------------------------------------------------------------------------+--+
| :ref:`hdf5_scheme <triqs__gfs__gf_meshLTbrillouin_zoneGT__hdf5_scheme>`                       |  |
+-----------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTbrillouin_zoneGT/constructor
    gf_meshLTbrillouin_zoneGT/index_to_point
    gf_meshLTbrillouin_zoneGT/index_to_linear
    gf_meshLTbrillouin_zoneGT/is_within_boundary
    gf_meshLTbrillouin_zoneGT/operator[]
    gf_meshLTbrillouin_zoneGT/operator==
    gf_meshLTbrillouin_zoneGT/modulo_reduce
    gf_meshLTbrillouin_zoneGT/locate_neighbours
    gf_meshLTbrillouin_zoneGT/get_interpolation_data
    gf_meshLTbrillouin_zoneGT/index_modulo
    gf_meshLTbrillouin_zoneGT/hdf5_scheme


Non Member functions
--------------------

+---------------------------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__gfs__gf_meshLTbrillouin_zoneGT__operatorLTLT>` |  |
+---------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTbrillouin_zoneGT/operator<<