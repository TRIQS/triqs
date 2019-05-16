..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_remove:

triqs::det_manip::det_manip::try_remove
=======================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | det_manip::value_type :red:`try_remove` (size_t :param:`i`, size_t :param:`j`)





Consider the removal the colj0 and row i0 from the matrix.

Returns the ratio of det Minv_new / det Minv.
This routine does NOT make any modification. It has to be completed with complete_operation().