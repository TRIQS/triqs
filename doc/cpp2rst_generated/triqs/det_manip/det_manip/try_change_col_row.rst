..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_try_change_col_row:

try_change_col_row
==================

**Synopsis**:

.. code-block:: c

    triqs::det_manip::det_manip::value_type try_change_col_row (size_t i, size_t j, const
   triqs::det_manip::det_manip::x_type & x, const triqs::det_manip::det_manip::y_type &
   y)

Consider the change the row i and column j and the corresponding x and y


Documentation
-------------


 Returns the ratio of det Minv_new / det Minv.
 This routine does NOT make any modification. It has to be completed with complete_operation().