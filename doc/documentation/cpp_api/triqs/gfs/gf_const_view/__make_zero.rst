..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_const_view____make_zero:

triqs::gfs::gf_const_view::__make_zero
======================================

*#include <triqs/gfs/gf_const_view.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | gf_const_view::zero_t :red:`__make_zero` (T , gf_const_view::data_t const & :param:`d`)





build a zero from a slice of data
   // MUST be static since it is used in constructors... (otherwise bug in clang)