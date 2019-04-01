..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_try_insert:

try_insert
==========

**Synopsis**:

.. code-block:: c

    triqs::det_manip::det_manip::value_type try_insert (size_t i, size_t j, const
   triqs::det_manip::det_manip::x_type & x, const triqs::det_manip::det_manip::y_type &
   y)

Insert operation at column j0 and row i0.


Documentation
-------------


 The operation consists in adding :

    * a column  f(x_i,    y_{j0})
    * and a row f(x_{i0}, x_j)

 The new column/row will be at col j0, row i0.

 0 <= i0,j0 <= N, where N is the current size of the matrix.
 The current column j0 (resp. row i0) will become column j0+1 (resp. row i0+1).
 Inserting at N simply add the new col at the end.
 Returns the ratio of det Minv_new / det Minv.

 This routine does NOT make any modification. It has to be completed with complete_operation().