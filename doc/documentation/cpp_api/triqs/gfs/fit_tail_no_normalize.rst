..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__fit_tail_no_normalize:

triqs::gfs::fit_tail_no_normalize
=================================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename V, typename T>`
       | auto :red:`fit_tail_no_normalize` (G<V, T> const & :param:`g`)





Tail-fit without normalization, returns moments rescaled by maximum frequency:  a_n * omega_max^n