..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_operator[]:

triqs::gfs::gf::operator[]
==========================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`pass a index_t of the mesh`
       | decltype(auto) :red:`operator[]` (gf::mesh_index_t const & :param:`arg`)

    2. | decltype(auto) :red:`operator[]` (gf::mesh_index_t const & :param:`arg`) const

    3. | :cppbrief:`pass a mesh_point of the mesh`
       | decltype(auto) :red:`operator[]` (gf::mesh_point_t const & :param:`x`)

    4. | decltype(auto) :red:`operator[]` (gf::mesh_point_t const & :param:`x`) const

    5. | :cppbrief:`pass an abtract closest_point. We extract the value of the domain from p, call the gf_closest_point trait`
       | :green:`template<typename U>`
       | decltype(auto) :red:`operator[]` (:ref:`closest_pt_wrap\<U...\> <triqs__gfs__closest_pt_wrap>` const & :param:`p`)

    6. | :green:`template<typename U>`
       | decltype(auto) :red:`operator[]` (:ref:`closest_pt_wrap\<U...\> <triqs__gfs__closest_pt_wrap>` const & :param:`p`) const

    7. | :cppbrief:`-------------- operator [] with tuple_com. Distinguich the lazy and non lazy case`
       | :green:`template<typename U>`
       | decltype(auto) :red:`operator[]` (:ref:`tuple_com\<U...\> <triqs__gfs__tuple_com>` const & :param:`tu`) &

    8. | :green:`template<typename U>`
       | decltype(auto) :red:`operator[]` (:ref:`tuple_com\<U...\> <triqs__gfs__tuple_com>` const & :param:`tu`) const &

    9. | :green:`template<typename U>`
       | decltype(auto) :red:`operator[]` (:ref:`tuple_com\<U...\> <triqs__gfs__tuple_com>` const & :param:`tu`) &&

    10. | :cppbrief:`------------- [] with lazy arguments -----------------------------`
        | :green:`template<typename Arg>`
        | clef::make_expr_subscript_t<gf<Var, Target> const &, Arg> :red:`operator[]` (Arg && :param:`arg`) const &

    11. | :green:`template<typename Arg>`
        | clef::make_expr_subscript_t<gf<Var, Target> &, Arg> :red:`operator[]` (Arg && :param:`arg`) &

    12. | :green:`template<typename Arg>`
        | clef::make_expr_subscript_t<gf<Var, Target>, Arg> :red:`operator[]` (Arg && :param:`arg`) &&

Documentation