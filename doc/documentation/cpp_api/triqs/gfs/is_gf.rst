..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__is_gf:

triqs::gfs::is_gf
=================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

     template<typename G, typename M0> class  :red:`is_gf`

Is G a gf, gf_view, gf_const_view
 // is_gf<G> is true iif G is a gf or a view
 // is_gf<G,M0> is true iif  G is a gf or a view and its mesh is M0