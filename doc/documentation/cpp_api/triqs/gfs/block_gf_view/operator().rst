..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _block_gf_view_operator():

triqs::gfs::block_gf_view::operator()
=====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`First, a simple () returns a view, like for an array...`
       | block_gf_view::const_view_type :red:`operator()` () const

    2. | :cppbrief:`Makes a view of *this if it is non const`
       | block_gf_view::view_type :red:`operator()` ()

    3. | decltype(auto) :red:`operator()` (int :param:`n`) const

    4. | :cppbrief:`Calls with at least one lazy argument : we make a clef expression, cf clef documentation`
       | :green:`template<typename Args>`
       | clef::make_expr_call_t<block_gf_view<Var, Target> &, Args...> :red:`operator()` (Args &&... :param:`args`) &

    5. | :green:`template<typename Args>`
       | clef::make_expr_call_t<block_gf_view<Var, Target> const &, Args...> :red:`operator()` (Args &&... :param:`args`) const &

    6. | :green:`template<typename Args>`
       | clef::make_expr_call_t<block_gf_view<Var, Target>, Args...> :red:`operator()` (Args &&... :param:`args`) &&

Documentation



 **1)**          Makes a const view of *this