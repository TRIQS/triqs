..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__mesh_pointLTlinear_meshLTDomainGTGT:

triqs::gfs::mesh_point<linear_mesh<Domain>>
===========================================

Defined in header <**>

.. code-block:: c

    template<typename Domain> class mesh_point<linear_mesh<Domain>>

---------------------------------------------------------------------------

                         The mesh point
     ---------------------------------------------------------------------------
                             The mesh point
     ---------------------------------------------------------------------------

Public members
--------------

+--------+--------------------------------------------------------------------------+--+
| m      | const triqs::gfs::mesh_point<linear_mesh<type-parameter-0-0> >::mesh_t * |  |
+--------+--------------------------------------------------------------------------+--+
| _index | triqs::gfs::mesh_point<linear_mesh<type-parameter-0-0> >::index_t        |  |
+--------+--------------------------------------------------------------------------+--+


Member types
------------

+---------+--------------------------+--+
| mesh_t  | linear_mesh<Domain>      |  |
+---------+--------------------------+--+
| index_t | typename mesh_t::index_t |  |
+---------+--------------------------+--+
| cast_t  | typename Domain::point_t |  |
+---------+--------------------------+--+


Member functions
----------------

+------------------------------------------------------------------------+--+
| :ref:`constructor <mesh_pointLTlinear_meshLTDomainGTGT_constructor>`   |  |
+------------------------------------------------------------------------+--+
| :ref:`advance <mesh_pointLTlinear_meshLTDomainGTGT_advance>`           |  |
+------------------------------------------------------------------------+--+
| :ref:`linear_index <mesh_pointLTlinear_meshLTDomainGTGT_linear_index>` |  |
+------------------------------------------------------------------------+--+
| :ref:`index <mesh_pointLTlinear_meshLTDomainGTGT_index>`               |  |
+------------------------------------------------------------------------+--+
| :ref:`at_end <mesh_pointLTlinear_meshLTDomainGTGT_at_end>`             |  |
+------------------------------------------------------------------------+--+
| :ref:`reset <mesh_pointLTlinear_meshLTDomainGTGT_reset>`               |  |
+------------------------------------------------------------------------+--+
| :ref:`mesh <mesh_pointLTlinear_meshLTDomainGTGT_mesh>`                 |  |
+------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    mesh_pointLTlinear_meshLTDomainGTGT/constructor
    mesh_pointLTlinear_meshLTDomainGTGT/advance
    mesh_pointLTlinear_meshLTDomainGTGT/linear_index
    mesh_pointLTlinear_meshLTDomainGTGT/index
    mesh_pointLTlinear_meshLTDomainGTGT/at_end
    mesh_pointLTlinear_meshLTDomainGTGT/reset
    mesh_pointLTlinear_meshLTDomainGTGT/mesh