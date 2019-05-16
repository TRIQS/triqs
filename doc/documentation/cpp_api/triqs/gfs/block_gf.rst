..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block_gf:

triqs::gfs::block_gf
====================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

     template<typename Var, typename Target> class  :red:`block_gf`

----------------------------------------------------------
   Declaration of main types : gf, gf_view, gf_const_view
--------------------------------------------------------


Public members
--------------

+------+-------------+--+
| name | std::string |  |
+------+-------------+--+


Member types
------------

+-------------------+----------------------------------------+---------------------------+
| variable_t        | Var                                    |                           |
+-------------------+----------------------------------------+---------------------------+
| target_t          | Target                                 |                           |
+-------------------+----------------------------------------+---------------------------+
| regular_type      | block_gf<Var, Target>                  |                           |
+-------------------+----------------------------------------+---------------------------+
| mutable_view_type | block_gf_view<Var, Target>             |                           |
+-------------------+----------------------------------------+---------------------------+
| view_type         | block_gf_view<Var, Target>             |                           |
+-------------------+----------------------------------------+---------------------------+
| const_view_type   | block_gf_const_view<Var, Target>       |                           |
+-------------------+----------------------------------------+---------------------------+
| real_t            | block_gf<Var, typename Target::real_t> |  The associated real type |
+-------------------+----------------------------------------+---------------------------+
| g_t               | gf<Var, Target>                        |                           |
+-------------------+----------------------------------------+---------------------------+
| data_t            | std::vector<g_t>                       |                           |
+-------------------+----------------------------------------+---------------------------+
| block_names_t     | std::vector<std::string>               |                           |
+-------------------+----------------------------------------+---------------------------+
| iterator          | iterator_impl<false>                   |                           |
+-------------------+----------------------------------------+---------------------------+
| const_iterator    | iterator_impl<true>                    |                           |
+-------------------+----------------------------------------+---------------------------+


Member functions
----------------

+----------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__block_gf__constructor>` |  |
+----------------------------------------------------------+--+
| :ref:`data <triqs__gfs__block_gf__data>`                 |  |
+----------------------------------------------------------+--+
| :ref:`block_names <triqs__gfs__block_gf__block_names>`   |  |
+----------------------------------------------------------+--+
| :ref:`_assign_impl <triqs__gfs__block_gf___assign_impl>` |  |
+----------------------------------------------------------+--+
| :ref:`operator= <triqs__gfs__block_gf__operator=>`       |  |
+----------------------------------------------------------+--+
| :ref:`rebind <triqs__gfs__block_gf__rebind>`             |  |
+----------------------------------------------------------+--+
| :ref:`operator() <triqs__gfs__block_gf__operator()>`     |  |
+----------------------------------------------------------+--+
| :ref:`operator[] <triqs__gfs__block_gf__operator[]>`     |  |
+----------------------------------------------------------+--+
| :ref:`hdf5_scheme <triqs__gfs__block_gf__hdf5_scheme>`   |  |
+----------------------------------------------------------+--+
| :ref:`serialize <triqs__gfs__block_gf__serialize>`       |  |
+----------------------------------------------------------+--+
| :ref:`begin <triqs__gfs__block_gf__begin>`               |  |
+----------------------------------------------------------+--+

.. toctree::
    :hidden:

    block_gf/constructor
    block_gf/data
    block_gf/block_names
    block_gf/_assign_impl
    block_gf/operator=
    block_gf/rebind
    block_gf/operator()
    block_gf/operator[]
    block_gf/hdf5_scheme
    block_gf/serialize
    block_gf/begin


Non Member functions
--------------------

+----------------------------------------------------------+--+
| :ref:`h5_write <triqs__gfs__block_gf__h5_write>`         |  |
+----------------------------------------------------------+--+
| :ref:`h5_read <triqs__gfs__block_gf__h5_read>`           |  |
+----------------------------------------------------------+--+
| :ref:`boost <triqs__gfs__block_gf__boost>`               |  |
+----------------------------------------------------------+--+
| :ref:`operator\<\< <triqs__gfs__block_gf__operatorLTLT>` |  |
+----------------------------------------------------------+--+

.. toctree::
    :hidden:

    block_gf/h5_write
    block_gf/h5_read
    block_gf/boost
    block_gf/operator<<