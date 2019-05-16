..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_remove2:

triqs::det_manip::det_manip::try_remove2
========================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | det_manip::value_type :red:`try_remove2` (size_t :param:`i0`, size_t :param:`i1`, size_t :param:`j0`, size_t :param:`j1`)





Double Removal operation of cols j0,j1 and rows i0,i1

Returns the ratio of det Minv_new / det Minv.
This routine does NOT make any modification. It has to be completed with complete_operation().