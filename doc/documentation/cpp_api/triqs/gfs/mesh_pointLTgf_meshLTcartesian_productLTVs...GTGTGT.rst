..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT:

triqs::gfs::mesh_point<gf_mesh<cartesian_product<Vs...>>>
=========================================================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

     template<typename Vs> class  :red:`mesh_point<gf_mesh<cartesian_product<Vs...>>>`

------------------------------------------------
The wrapper for the mesh point


Public members
--------------

+--------+---------------------------------------------------------------------------------------------+--+
| m      | const triqs::gfs::mesh_point<gf_mesh<cartesian_product<type-parameter-0-0...> > >::mesh_t * |  |
+--------+---------------------------------------------------------------------------------------------+--+
| _c     | triqs::gfs::mesh_point<gf_mesh<cartesian_product<type-parameter-0-0...> > >::m_pt_tuple_t   |  |
+--------+---------------------------------------------------------------------------------------------+--+
| _atend | bool                                                                                        |  |
+--------+---------------------------------------------------------------------------------------------+--+


Member types
------------

+----------------+------------------------------------------------------------------------------+--+
| mesh_t         | gf_mesh<cartesian_product<Vs...> >                                           |  |
+----------------+------------------------------------------------------------------------------+--+
| index_t        | typename mesh_t::index_t                                                     |  |
+----------------+------------------------------------------------------------------------------+--+
| m_pt_tuple_t   | typename mesh_t::m_pt_tuple_t                                                |  |
+----------------+------------------------------------------------------------------------------+--+
| linear_index_t | typename mesh_t::linear_index_t                                              |  |
+----------------+------------------------------------------------------------------------------+--+
| domain_pt_t    | typename mesh_t::domain_pt_t                                                 |  |
+----------------+------------------------------------------------------------------------------+--+
| cast_t         | mesh_point<gf_mesh<cartesian_product<type-parameter-0-0...> > >::domain_pt_t |  |
+----------------+------------------------------------------------------------------------------+--+


Member functions
----------------

+-----------------------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT__constructor>` |  |
+-----------------------------------------------------------------------------------------------------+--+
| :ref:`advance <triqs__gfs__mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT__advance>`           |  |
+-----------------------------------------------------------------------------------------------------+--+
| :ref:`at_end <triqs__gfs__mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT__at_end>`             |  |
+-----------------------------------------------------------------------------------------------------+--+
| :ref:`get <triqs__gfs__mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT__get>`                   |  |
+-----------------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/constructor
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/advance
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/at_end
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/get