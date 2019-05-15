..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__gf_meshLTcartesian_productLTVs___GTGT:

triqs::gfs::gf_mesh<cartesian_product<Vs...>>
=============================================

Defined in header <**>

.. code-block:: c

    template<typename Vs> class gf_mesh<cartesian_product<Vs...>>

Cartesian product of meshes

     the mesh is simply a cartesian product
         the mesh is simply a cartesian product

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

+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`constructor <gf_meshLTcartesian_productLTVs___GTGT_constructor>`                                   | -------------------- Constructors -------------------                                     |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`operator== <gf_meshLTcartesian_productLTVs___GTGT_operator==>`                                     | Mesh comparison                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`operator!= <gf_meshLTcartesian_productLTVs___GTGT_operator!=>`                                     |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`domain <gf_meshLTcartesian_productLTVs___GTGT_domain>`                                             | -------------------- Accessors (from concept) -------------------                         |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`size <gf_meshLTcartesian_productLTVs___GTGT_size>`                                                 | size of the mesh is the product of size                                                   |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`size_of_components <gf_meshLTcartesian_productLTVs___GTGT_size_of_components>`                     | The sizes of all mesh components                                                          |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`index_to_point <gf_meshLTcartesian_productLTVs___GTGT_index_to_point>`                             | Conversions point <-> index <-> linear_index                                              |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`index_to_linear <gf_meshLTcartesian_productLTVs___GTGT_index_to_linear>`                           | The linear_index is the tuple of the linear_index of the components                       |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`components <gf_meshLTcartesian_productLTVs___GTGT_components>`                                     | -------------------- Accessors (other) -------------------                                |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`get <gf_meshLTcartesian_productLTVs___GTGT_get>`                                                   | for structured binding                                                                    |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`mp_to_linear <gf_meshLTcartesian_productLTVs___GTGT_mp_to_linear>`                                 |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`mesh_pt_components_to_linear <gf_meshLTcartesian_productLTVs___GTGT_mesh_pt_components_to_linear>` | Same but a variadic list of mesh_point_t                                                  |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`operator[] <gf_meshLTcartesian_productLTVs___GTGT_operator[]>`                                     | Accessing a point of the mesh                                                             |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`operator() <gf_meshLTcartesian_productLTVs___GTGT_operator()>`                                     |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`begin <gf_meshLTcartesian_productLTVs___GTGT_begin>`                                               |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`end <gf_meshLTcartesian_productLTVs___GTGT_end>`                                                   |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`cbegin <gf_meshLTcartesian_productLTVs___GTGT_cbegin>`                                             |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`cend <gf_meshLTcartesian_productLTVs___GTGT_cend>`                                                 |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`is_within_boundary <gf_meshLTcartesian_productLTVs___GTGT_is_within_boundary>`                     | Is the point of evaluation in the mesh. All components must be in the corresponding mesh. |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`get_interpolation_data <gf_meshLTcartesian_productLTVs___GTGT_get_interpolation_data>`             | not implemented, makes no sense                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`evaluate <gf_meshLTcartesian_productLTVs___GTGT_evaluate>`                                         |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`hdf5_scheme <gf_meshLTcartesian_productLTVs___GTGT_hdf5_scheme>`                                   | -------------------- HDF5 -------------------                                             |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+
| :ref:`serialize <gf_meshLTcartesian_productLTVs___GTGT_serialize>`                                       |                                                                                           |
+----------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTcartesian_productLTVs...GTGT/constructor
    gf_meshLTcartesian_productLTVs...GTGT/operator==
    gf_meshLTcartesian_productLTVs...GTGT/operator!=
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
    gf_meshLTcartesian_productLTVs...GTGT/operator()
    gf_meshLTcartesian_productLTVs...GTGT/begin
    gf_meshLTcartesian_productLTVs...GTGT/end
    gf_meshLTcartesian_productLTVs...GTGT/cbegin
    gf_meshLTcartesian_productLTVs...GTGT/cend
    gf_meshLTcartesian_productLTVs...GTGT/is_within_boundary
    gf_meshLTcartesian_productLTVs...GTGT/get_interpolation_data
    gf_meshLTcartesian_productLTVs...GTGT/evaluate
    gf_meshLTcartesian_productLTVs...GTGT/hdf5_scheme
    gf_meshLTcartesian_productLTVs...GTGT/serialize


Non Member functions
--------------------

+--------------------------------------------------------------------------+-------------------------------------------------+
| :ref:`h5_write <gf_meshLTcartesian_productLTVs___GTGT_h5_write>`         | Write into HDF5                                 |
+--------------------------------------------------------------------------+-------------------------------------------------+
| :ref:`h5_read <gf_meshLTcartesian_productLTVs___GTGT_h5_read>`           | Read from HDF5                                  |
+--------------------------------------------------------------------------+-------------------------------------------------+
| :ref:`boost <gf_meshLTcartesian_productLTVs___GTGT_boost>`               |                                                 |
+--------------------------------------------------------------------------+-------------------------------------------------+
| :ref:`operator\<\< <gf_meshLTcartesian_productLTVs___GTGT_operatorLTLT>` | -------------------- print  ------------------- |
+--------------------------------------------------------------------------+-------------------------------------------------+

.. toctree::
    :hidden:

    gf_meshLTcartesian_productLTVs...GTGT/h5_write
    gf_meshLTcartesian_productLTVs...GTGT/h5_read
    gf_meshLTcartesian_productLTVs...GTGT/boost
    gf_meshLTcartesian_productLTVs...GTGT/operator<<