..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mesh_point_compatible_to_mesh_impl:

mesh_point_compatible_to_mesh_impl
==================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`special case for cartesian product, it must be checked for every component.`
       | :green:`template<typename MP, typename M, size_t Is>`
       | bool :red:`mesh_point_compatible_to_mesh_impl` (std::index_sequence<Is...> ,
       |   MP const & :param:`mp`,
       |   :ref:`gf_mesh\<cartesian_product\<M...\> \> <triqs__gfs__gf_mesh>` const & :param:`m`)