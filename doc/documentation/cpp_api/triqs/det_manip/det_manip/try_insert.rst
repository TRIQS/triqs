..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_insert:

triqs::det_manip::det_manip::try_insert
=======================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | det_manip::value_type :red:`try_insert` (size_t :param:`i`,
       |             size_t :param:`j`,
       |             det_manip::x_type const & :param:`x`,
       |             det_manip::y_type const & :param:`y`)





Insert operation at column j0 and row i0.

The operation consists in adding :

   * a column  f(x_i,    y_{j0})
   * and a row f(x_{i0}, x_j)

The new column/row will be at col j0, row i0.

0 <= i0,j0 <= N, where N is the current size of the matrix.
The current column j0 (resp. row i0) will become column j0+1 (resp. row i0+1).
Inserting at N simply add the new col at the end.
Returns the ratio of det Minv_new / det Minv.

This routine does NOT make any modification. It has to be completed with complete_operation().





Parameters
^^^^^^^^^^

 * :param:`i`

 * :param:`j`