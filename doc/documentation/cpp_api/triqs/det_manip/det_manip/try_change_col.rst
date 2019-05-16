..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_change_col:

triqs::det_manip::det_manip::try_change_col
===========================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | det_manip::value_type :red:`try_change_col` (size_t :param:`j`, det_manip::y_type const & :param:`y`)





Consider the change the column j and the corresponding y.

Returns the ratio of det Minv_new / det Minv.
This routine does NOT make any modification. It has to be completed with complete_operation().