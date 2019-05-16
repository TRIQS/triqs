..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_change_row:

triqs::det_manip::det_manip::try_change_row
===========================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | det_manip::value_type :red:`try_change_row` (size_t :param:`i`, det_manip::x_type const & :param:`x`)





Consider the change the row i and the corresponding x.

Returns the ratio of det Minv_new / det Minv.
This routine does NOT make any modification. It has to be completed with complete_operation().