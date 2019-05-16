..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__foreach:

triqs::det_manip::det_manip::foreach
====================================

*#include <triqs/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename LambdaType>`
       | void :red:`foreach` (det_manip<FunctionType> const & :param:`d`, LambdaType const & :param:`f`)





Given a lambda f : x,y,M, it calls f(x_i,y_j,M_ji) for all i,j
     // Order of iteration is NOT fixed, it is optimised (for memory traversal)