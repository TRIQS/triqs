..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__block2_gf__operator():

triqs::gfs::block2_gf::operator()
=================================

*#include <triqs/gfs/block2_gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | block2_gf::const_view_type :red:`operator()` () const

    2. | block2_gf::view_type :red:`operator()` ()

    3. | :green:`template<typename Args>`
       | clef::make_expr_call_t<block2_gf<Var, Target> &, Args...> :red:`operator()` (Args &&... :param:`args`) &

Documentation



 **1)**   First, a simple () returns a view, like for an array...
Makes a const view of *this



 **2)**   Makes a view of *this if it is non const



 **3)**   Calls with at least one lazy argument : we make a clef expression, cf clef documentation