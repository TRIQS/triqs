..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_refill:

triqs::det_manip::det_manip::try_refill
=======================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename ArgumentContainer1, typename ArgumentContainer2>`
       | det_manip::value_type :red:`try_refill` (ArgumentContainer1 const & :param:`X`, ArgumentContainer2 const & :param:`Y`)





Refill determinant with new values

New values are calculated as f(x_i, y_i)

Returns the ratio of det Minv_new / det Minv.

This routine does NOT make any modification. It has to be completed with complete_operation().