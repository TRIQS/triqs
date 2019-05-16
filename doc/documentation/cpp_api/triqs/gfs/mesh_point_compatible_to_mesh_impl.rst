..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__mesh_point_compatible_to_mesh_impl:

triqs::gfs::mesh_point_compatible_to_mesh_impl
==============================================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename MP, typename M, size_t Is>`
       | bool :red:`mesh_point_compatible_to_mesh_impl` (std::index_sequence<Is...> ,
       |                                     MP const & :param:`mp`,
       |                                     :ref:`gf_mesh\<cartesian_product\<M...\> \> <triqs__gfs__gf_mesh>` const & :param:`m`)





special case for cartesian product, it must be checked for every component.