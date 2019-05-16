..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__tail_eval:

triqs::gfs::tail_eval
=====================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<int R>`
       | auto :red:`tail_eval` (arrays::array_const_view<dcomplex, R> :param:`A`, dcomplex :param:`om`)





Computes sum A_n / om^n
 // Return array<dcomplex, R -1 > if R>1 else dcomplex
 // FIXME : use dynamic R array when available.
 // FIXME : array of dimension 0