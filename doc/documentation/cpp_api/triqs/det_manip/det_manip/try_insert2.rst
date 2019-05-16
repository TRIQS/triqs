..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__try_insert2:

triqs::det_manip::det_manip::try_insert2
========================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | det_manip::value_type :red:`try_insert2` (size_t :param:`i0`,
       |              size_t :param:`i1`,
       |              size_t :param:`j0`,
       |              size_t :param:`j1`,
       |              det_manip::x_type const & :param:`x0_`,
       |              det_manip::x_type const & :param:`x1_`,
       |              det_manip::y_type const & :param:`y0_`,
       |              det_manip::y_type const & :param:`y1_`)





Double Insert operation at colum j0,j1 and row i0,i1.

The operation consists in adding :
   * two columns  f(x_i,    y_{j0}), f(x_i,    y_{j1})
   * and two rows f(x_{i0}, x_j),    f(x_{i1}, x_j)
The new colums/rows will be at col j0, j1, row i0, i1.

0 <= i0,i1,j0,j1 <= N+1, where N is the current size of the matrix.
Returns the ratio of det Minv_new / det Minv.
This routine does NOT make any modification. It has to be completed with complete_operation().