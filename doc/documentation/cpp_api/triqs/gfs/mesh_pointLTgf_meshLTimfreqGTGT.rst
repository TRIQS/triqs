..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__mesh_pointLTgf_meshLTimfreqGTGT:

triqs::gfs::mesh_point<gf_mesh<imfreq>>
=======================================

Defined in header <**>

.. code-block:: c

     class mesh_point<gf_mesh<imfreq>>

---------------------------------------------------------------------------

                       The mesh point
    NB : the mesh point is also in this case a matsubara_freq.
   ---------------------------------------------------------------------------
                           The mesh point
    NB : the mesh point is also in this case a matsubara_freq.
   ---------------------------------------------------------------------------

Public members
--------------

+-----------+----------------------------+--+
| n         | long                       |  |
+-----------+----------------------------+--+
| beta      | double                     |  |
+-----------+----------------------------+--+
| statistic | triqs::gfs::statistic_enum |  |
+-----------+----------------------------+--+


Member types
------------

+---------+-----------------------------------+--+
| index_t | typename gf_mesh<imfreq>::index_t |  |
+---------+-----------------------------------+--+


Member functions
----------------

+--------------------------------------------------------------------+--+
| :ref:`constructor <mesh_pointLTgf_meshLTimfreqGTGT_constructor>`   |  |
+--------------------------------------------------------------------+--+
| :ref:`advance <mesh_pointLTgf_meshLTimfreqGTGT_advance>`           |  |
+--------------------------------------------------------------------+--+
| :ref:`linear_index <mesh_pointLTgf_meshLTimfreqGTGT_linear_index>` |  |
+--------------------------------------------------------------------+--+
| :ref:`index <mesh_pointLTgf_meshLTimfreqGTGT_index>`               |  |
+--------------------------------------------------------------------+--+
| :ref:`at_end <mesh_pointLTgf_meshLTimfreqGTGT_at_end>`             |  |
+--------------------------------------------------------------------+--+
| :ref:`reset <mesh_pointLTgf_meshLTimfreqGTGT_reset>`               |  |
+--------------------------------------------------------------------+--+
| :ref:`mesh <mesh_pointLTgf_meshLTimfreqGTGT_mesh>`                 |  |
+--------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    mesh_pointLTgf_meshLTimfreqGTGT/constructor
    mesh_pointLTgf_meshLTimfreqGTGT/advance
    mesh_pointLTgf_meshLTimfreqGTGT/linear_index
    mesh_pointLTgf_meshLTimfreqGTGT/index
    mesh_pointLTgf_meshLTimfreqGTGT/at_end
    mesh_pointLTgf_meshLTimfreqGTGT/reset
    mesh_pointLTgf_meshLTimfreqGTGT/mesh