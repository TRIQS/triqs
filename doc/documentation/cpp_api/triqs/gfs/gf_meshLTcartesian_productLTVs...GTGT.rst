..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT:

triqs::gfs::gf_mesh<cartesian_product<Vs...>>
=============================================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

     template<typename Vs> class  :red:`gf_mesh<cartesian_product<Vs...>>`

Cartesian product of meshes
   // the mesh is simply a cartesian product


Member types
------------

+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| domain_t       | domain_product<typename gf_mesh<Vs>::domain_t...>                      |                                                      |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| m_tuple_t      | std::tuple<gf_mesh<Vs>...>                                             |                                                      |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| ms_tuple_t     | std::tuple<Vs...>                                                      |                                                      |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| m_pt_tuple_t   | std::tuple<typename gf_mesh<Vs>::mesh_point_t...>                      |                                                      |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| domain_pt_t    | typename domain_t::point_t                                             |                                                      |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| linear_index_t | std::tuple<typename gf_mesh<Vs>::linear_index_t...>                    |                                                      |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| var_t          | cartesian_product<Vs...>                                               |                                                      |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| mesh_point_t   | mesh_point<gf_mesh<cartesian_product<type-parameter-0-0...> > >        |  -------------------- mesh_point ------------------- |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+
| const_iterator | mesh_pt_generator<gf_mesh<cartesian_product<type-parameter-0-0...> > > |  Iterating on all the points...                      |
+----------------+------------------------------------------------------------------------+------------------------------------------------------+


Member functions
----------------

+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__constructor>`                                 |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`operator== <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__operator==>`                                     |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`domain <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__domain>`                                             |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`size <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__size>`                                                 |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`size_of_components <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__size_of_components>`                     |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`index_to_point <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__index_to_point>`                             |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`index_to_linear <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__index_to_linear>`                           |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`components <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__components>`                                     |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`get <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__get>`                                                   |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`mp_to_linear <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__mp_to_linear>`                                 |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`mesh_pt_components_to_linear <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__mesh_pt_components_to_linear>` |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`operator[] <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__operator[]>`                                     |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`is_within_boundary <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__is_within_boundary>`                     |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`get_interpolation_data <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__get_interpolation_data>`             |  |
+-----------------------------------------------------------------------------------------------------------------------+--+
| :ref:`hdf5_scheme <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__hdf5_scheme>`                                   |  |
+-----------------------------------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTcartesian_productLTVs...GTGT/constructor
    gf_meshLTcartesian_productLTVs...GTGT/operator==
    gf_meshLTcartesian_productLTVs...GTGT/domain
    gf_meshLTcartesian_productLTVs...GTGT/size
    gf_meshLTcartesian_productLTVs...GTGT/size_of_components
    gf_meshLTcartesian_productLTVs...GTGT/index_to_point
    gf_meshLTcartesian_productLTVs...GTGT/index_to_linear
    gf_meshLTcartesian_productLTVs...GTGT/components
    gf_meshLTcartesian_productLTVs...GTGT/get
    gf_meshLTcartesian_productLTVs...GTGT/mp_to_linear
    gf_meshLTcartesian_productLTVs...GTGT/mesh_pt_components_to_linear
    gf_meshLTcartesian_productLTVs...GTGT/operator[]
    gf_meshLTcartesian_productLTVs...GTGT/is_within_boundary
    gf_meshLTcartesian_productLTVs...GTGT/get_interpolation_data
    gf_meshLTcartesian_productLTVs...GTGT/hdf5_scheme


Non Member functions
--------------------

+---------------------------------------------------------------------------------------+--+
| :ref:`h5_write <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__h5_write>`         |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`h5_read <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__h5_read>`           |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`boost <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__boost>`               |  |
+---------------------------------------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT__operatorLTLT>` |  |
+---------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_meshLTcartesian_productLTVs...GTGT/h5_write
    gf_meshLTcartesian_productLTVs...GTGT/h5_read
    gf_meshLTcartesian_productLTVs...GTGT/boost
    gf_meshLTcartesian_productLTVs...GTGT/operator<<