..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mesh_point_compatible_to_mesh:

mesh_point_compatible_to_mesh
=============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`check if mesh_point mp is compatible with mesh m. Intented for debug mode only.`
       | :green:`template<typename MP, typename T>`
       | bool :red:`mesh_point_compatible_to_mesh` (MP const & :param:`mp`, :ref:`gf_mesh\<T\> <triqs__gfs__gf_mesh>` const & :param:`m`)

    2. | :cppbrief:`special case for imtime : do not check the statistic !`
       | :green:`template<typename MP>`
       | bool :red:`mesh_point_compatible_to_mesh` (MP const & :param:`mp`, :ref:`gf_mesh\<imtime\> <triqs__gfs__gf_meshLTimtimeGT>` const & :param:`m`)

Documentation