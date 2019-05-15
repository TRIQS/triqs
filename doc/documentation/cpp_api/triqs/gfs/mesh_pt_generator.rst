..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__mesh_pt_generator:

triqs::gfs::mesh_pt_generator
=============================

Defined in header <**>

.. code-block:: c

    template<typename MeshType> class mesh_pt_generator

FIXME : remove boost !




Member functions
----------------

+----------------------------------------------------+----------------------------------------------------------------------------------------+
| :ref:`constructor <mesh_pt_generator_constructor>` | do NOT check = of mesh, otherwise e.g. block iterator does not work (infinite loop...) |
+----------------------------------------------------+----------------------------------------------------------------------------------------+
| :ref:`increment <mesh_pt_generator_increment>`     |                                                                                        |
+----------------------------------------------------+----------------------------------------------------------------------------------------+
| :ref:`at_end <mesh_pt_generator_at_end>`           |                                                                                        |
+----------------------------------------------------+----------------------------------------------------------------------------------------+
| :ref:`to_point <mesh_pt_generator_to_point>`       |                                                                                        |
+----------------------------------------------------+----------------------------------------------------------------------------------------+
| :ref:`operator+= <mesh_pt_generator_operator+=>`   |                                                                                        |
+----------------------------------------------------+----------------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    mesh_pt_generator/constructor
    mesh_pt_generator/increment
    mesh_pt_generator/at_end
    mesh_pt_generator/to_point
    mesh_pt_generator/operator+=


Non Member functions
--------------------

+------------------------------------------------+--+
| :ref:`itertools <mesh_pt_generator_itertools>` |  |
+------------------------------------------------+--+
| :ref:`operator+ <mesh_pt_generator_operator+>` |  |
+------------------------------------------------+--+

.. toctree::
    :hidden:

    mesh_pt_generator/itertools
    mesh_pt_generator/operator+