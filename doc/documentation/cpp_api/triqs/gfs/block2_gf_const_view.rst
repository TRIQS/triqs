..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block2_gf_const_view:

triqs::gfs::block2_gf_const_view
================================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

     template<typename Var, typename Target> class  :red:`block2_gf_const_view`

----------------------  block2_gf_const_view -----------------------------------------
block2_gf_const_view


Public members
--------------

+------+-------------+--+
| name | std::string |  |
+------+-------------+--+


Member types
------------

+-------------------+----------------------------------------------------+---------------------------+
| variable_t        | Var                                                |                           |
+-------------------+----------------------------------------------------+---------------------------+
| target_t          | Target                                             |                           |
+-------------------+----------------------------------------------------+---------------------------+
| regular_type      | block2_gf<Var, Target>                             |                           |
+-------------------+----------------------------------------------------+---------------------------+
| mutable_view_type | block2_gf_view<Var, Target>                        |                           |
+-------------------+----------------------------------------------------+---------------------------+
| view_type         | block2_gf_const_view<Var, Target>                  |                           |
+-------------------+----------------------------------------------------+---------------------------+
| const_view_type   | block2_gf_const_view<Var, Target>                  |                           |
+-------------------+----------------------------------------------------+---------------------------+
| real_t            | block2_gf_const_view<Var, typename Target::real_t> |  The associated real type |
+-------------------+----------------------------------------------------+---------------------------+
| g_t               | gf_const_view<Var, Target>                         |                           |
+-------------------+----------------------------------------------------+---------------------------+
| data_t            | std::vector<std::vector<g_t> >                     |                           |
+-------------------+----------------------------------------------------+---------------------------+
| block_names_t     | std::vector<std::vector<std::string> >             |                           |
+-------------------+----------------------------------------------------+---------------------------+
| iterator          | iterator_impl<false>                               |                           |
+-------------------+----------------------------------------------------+---------------------------+
| const_iterator    | iterator_impl<true>                                |                           |
+-------------------+----------------------------------------------------+---------------------------+


Member functions
----------------

+----------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__block2_gf_const_view__constructor>` |  |
+----------------------------------------------------------------------+--+
| :ref:`data <triqs__gfs__block2_gf_const_view__data>`                 |  |
+----------------------------------------------------------------------+--+
| :ref:`block_names <triqs__gfs__block2_gf_const_view__block_names>`   |  |
+----------------------------------------------------------------------+--+
| :ref:`_assign_impl <triqs__gfs__block2_gf_const_view___assign_impl>` |  |
+----------------------------------------------------------------------+--+
| :ref:`rebind <triqs__gfs__block2_gf_const_view__rebind>`             |  |
+----------------------------------------------------------------------+--+
| :ref:`operator() <triqs__gfs__block2_gf_const_view__operator()>`     |  |
+----------------------------------------------------------------------+--+
| :ref:`operator[] <triqs__gfs__block2_gf_const_view__operator[]>`     |  |
+----------------------------------------------------------------------+--+
| :ref:`hdf5_scheme <triqs__gfs__block2_gf_const_view__hdf5_scheme>`   |  |
+----------------------------------------------------------------------+--+
| :ref:`serialize <triqs__gfs__block2_gf_const_view__serialize>`       |  |
+----------------------------------------------------------------------+--+
| :ref:`begin <triqs__gfs__block2_gf_const_view__begin>`               |  |
+----------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    block2_gf_const_view/constructor
    block2_gf_const_view/data
    block2_gf_const_view/block_names
    block2_gf_const_view/_assign_impl
    block2_gf_const_view/rebind
    block2_gf_const_view/operator()
    block2_gf_const_view/operator[]
    block2_gf_const_view/hdf5_scheme
    block2_gf_const_view/serialize
    block2_gf_const_view/begin


Non Member functions
--------------------

+----------------------------------------------------------------------+--+
| :ref:`h5_write <triqs__gfs__block2_gf_const_view__h5_write>`         |  |
+----------------------------------------------------------------------+--+
| :ref:`h5_read <triqs__gfs__block2_gf_const_view__h5_read>`           |  |
+----------------------------------------------------------------------+--+
| :ref:`boost <triqs__gfs__block2_gf_const_view__boost>`               |  |
+----------------------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__gfs__block2_gf_const_view__operatorLTLT>` |  |
+----------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    block2_gf_const_view/h5_write
    block2_gf_const_view/h5_read
    block2_gf_const_view/boost
    block2_gf_const_view/operator<<