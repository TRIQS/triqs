..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf____make_zero:

triqs::gfs::gf::__make_zero
===========================

*#include <triqs/gfs/gf.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename T>`
       | gf::zero_t :red:`__make_zero` (T , gf::data_t const & :param:`d`)





build a zero from a slice of data
   // MUST be static since it is used in constructors... (otherwise bug in clang)