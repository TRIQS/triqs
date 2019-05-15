..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT:

triqs::gfs::mesh_point<gf_mesh<cartesian_product<Vs...>>>
=========================================================

Defined in header <**>

.. code-block:: c

    template<typename Vs> class mesh_point<gf_mesh<cartesian_product<Vs...>>>

------------------------------------------------

     The wrapper for the mesh point
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

+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`constructor <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_constructor>`           |                                                                                                             |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`components_tuple <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_components_tuple>` |                                                                                                             |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`linear_index <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_linear_index>`         |                                                                                                             |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`index <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_index>`                       |                                                                                                             |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`mesh <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_mesh>`                         |                                                                                                             |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`advance <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_advance>`                   | index[0] +=1; if index[0]==m.component[0].size() { index[0]=0; index[1] +=1; if  ....}  and so on until dim |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`at_end <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_at_end>`                     | index_t index() const { return _index;}  not implemented yet                                                |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`reset <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_reset>`                       |                                                                                                             |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+
| :ref:`get <mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT_get>`                           | std::get should work FIXME ? redondant                                                                      |
+------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/constructor
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/components_tuple
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/linear_index
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/index
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/mesh
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/advance
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/at_end
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/reset
    mesh_pointLTgf_meshLTcartesian_productLTVs...GTGTGT/get