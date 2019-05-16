..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_change_col_row:

triqs::det_manip::det_manip::try_change_col_row
===============================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | det_manip::value_type :red:`try_change_col_row` (size_t :param:`i`,
       |                     size_t :param:`j`,
       |                     det_manip::x_type const & :param:`x`,
       |                     det_manip::y_type const & :param:`y`)





Consider the change the row i and column j and the corresponding x and y

Returns the ratio of det Minv_new / det Minv.
This routine does NOT make any modification. It has to be completed with complete_operation().