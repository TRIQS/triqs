..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_h5_rw__write:

triqs::gfs::gf_h5_rw::write
===========================

*#include <triqs/gfs/gf_h5_rw.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename G>`
       | void :red:`write` (h5::group :param:`gr`, G const & :param:`g`)





template <typename G> static void write(h5::group gr, G const &g) { _write(gr, gf_h5_before_write<V, T>::invoke(gr, g)); }