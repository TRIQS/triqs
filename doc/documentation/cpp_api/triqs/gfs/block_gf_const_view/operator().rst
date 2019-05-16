..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block_gf_const_view__operator():

triqs::gfs::block_gf_const_view::operator()
===========================================

*#include <triqs/gfs/block_gf_const_view.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | block_gf_const_view::const_view_type :red:`operator()` () const

    2. | block_gf_const_view::view_type :red:`operator()` ()

    3. | :green:`template<typename Args>`
       | clef::make_expr_call_t<block_gf_const_view<Var, Target> &, Args...> :red:`operator()` (Args &&... :param:`args`) &

Documentation



 **1)**   First, a simple () returns a view, like for an array...
Makes a const view of *this



 **2)**   Makes a view of *this if it is non const



 **3)**   Calls with at least one lazy argument : we make a clef expression, cf clef documentation