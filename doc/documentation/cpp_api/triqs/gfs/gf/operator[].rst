..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf__operator[]:

triqs::gfs::gf::operator[]
==========================

*#include <triqs/gfs/gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | decltype(auto) :red:`operator[]` (gf::mesh_index_t const & :param:`arg`)

    2. | decltype(auto) :red:`operator[]` (gf::mesh_point_t const & :param:`x`)

    3. | :green:`template<typename U>`
       | decltype(auto) :red:`operator[]` (:ref:`closest_pt_wrap\<U...\> <triqs__gfs__closest_pt_wrap>` const & :param:`p`)

    4. | :green:`template<typename U>`
       | decltype(auto) :red:`operator[]` (:ref:`tuple_com\<U...\> <triqs__gfs__tuple_com>` const & :param:`tu`) &

    5. | :green:`template<typename Arg>`
       | clef::make_expr_subscript_t<this_t const &, Arg> :red:`operator[]` (Arg && :param:`arg`) const &

Documentation



 **1)**   pass a index_t of the mesh



 **2)**   pass a mesh_point of the mesh



 **3)**   pass an abtract closest_point. We extract the value of the domain from p, call the gf_closest_point trait



 **4)**   -------------- operator [] with tuple_com. Distinguich the lazy and non lazy case



 **5)**   ------------- [] with lazy arguments -----------------------------