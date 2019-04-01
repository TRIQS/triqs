..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_try_insert2:

try_insert2
===========

**Synopsis**:

.. code-block:: c

    triqs::det_manip::det_manip::value_type try_insert2 (size_t i0, size_t i1, size_t j0,
   size_t j1, const triqs::det_manip::det_manip::x_type & x0_, const
   triqs::det_manip::det_manip::x_type & x1_, const
   triqs::det_manip::det_manip::y_type & y0_, const
   triqs::det_manip::det_manip::y_type & y1_)

Double Insert operation at colum j0,j1 and row i0,i1.


Documentation
-------------


 The operation consists in adding :
    * two columns  f(x_i,    y_{j0}), f(x_i,    y_{j1})
    * and two rows f(x_{i0}, x_j),    f(x_{i1}, x_j)
 The new colums/rows will be at col j0, j1, row i0, i1.

 0 <= i0,i1,j0,j1 <= N+1, where N is the current size of the matrix.
 Returns the ratio of det Minv_new / det Minv.
 This routine does NOT make any modification. It has to be completed with complete_operation().