..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _block_gf_view_operator[]:

triqs::gfs::block_gf_view::operator[]
=====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`------------- All the [] operators without lazy arguments -----------------------------`
       | decltype(auto) :red:`operator[]` (int :param:`n`) const

    2. | decltype(auto) :red:`operator[]` (int :param:`n`)

    3. | :cppbrief:`------------- [] with lazy arguments -----------------------------`
       | :green:`template<typename Arg>`
       | clef::make_expr_subscript_t<block_gf_view<Var, Target> const &, Arg> :red:`operator[]` (Arg && :param:`arg`) const &

    4. | :green:`template<typename Arg>`
       | clef::make_expr_subscript_t<block_gf_view<Var, Target> &, Arg> :red:`operator[]` (Arg && :param:`arg`) &

    5. | :green:`template<typename Arg>`
       | clef::make_expr_subscript_t<block_gf_view<Var, Target>, Arg> :red:`operator[]` (Arg && :param:`arg`) &&

Documentation