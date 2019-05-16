..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTcyclic_latticeGT:

triqs::gfs::gf_mesh<cyclic_lattice>
===================================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

      class  :red:`gf_mesh<cyclic_lattice>`

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

+---------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__gf_meshLTcyclic_latticeGT__constructor>`             |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`index_to_point <triqs__gfs__gf_meshLTcyclic_latticeGT__index_to_point>`         |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`index_to_linear <triqs__gfs__gf_meshLTcyclic_latticeGT__index_to_linear>`       |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`is_within_boundary <triqs__gfs__gf_meshLTcyclic_latticeGT__is_within_boundary>` |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`operator[] <triqs__gfs__gf_meshLTcyclic_latticeGT__operator[]>`                 |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`operator== <triqs__gfs__gf_meshLTcyclic_latticeGT__operator==>`                 |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`modulo_reduce <triqs__gfs__gf_meshLTcyclic_latticeGT__modulo_reduce>`           |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`locate_neighbours <triqs__gfs__gf_meshLTcyclic_latticeGT__locate_neighbours>`   |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`index_modulo <triqs__gfs__gf_meshLTcyclic_latticeGT__index_modulo>`             |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`hdf5_scheme <triqs__gfs__gf_meshLTcyclic_latticeGT__hdf5_scheme>`               |  |
+---------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTcyclic_latticeGT/constructor
    gf_meshLTcyclic_latticeGT/index_to_point
    gf_meshLTcyclic_latticeGT/index_to_linear
    gf_meshLTcyclic_latticeGT/is_within_boundary
    gf_meshLTcyclic_latticeGT/operator[]
    gf_meshLTcyclic_latticeGT/operator==
    gf_meshLTcyclic_latticeGT/modulo_reduce
    gf_meshLTcyclic_latticeGT/locate_neighbours
    gf_meshLTcyclic_latticeGT/index_modulo
    gf_meshLTcyclic_latticeGT/hdf5_scheme


Non Member functions
--------------------

+---------------------------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__gfs__gf_meshLTcyclic_latticeGT__operatorLTLT>` |  |
+---------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTcyclic_latticeGT/operator<<