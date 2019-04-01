..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_try_change_col:

try_change_col
==============

**Synopsis**:

.. code-block:: c

    triqs::det_manip::det_manip::value_type try_change_col (size_t j, const
   triqs::det_manip::det_manip::y_type & y)

Consider the change the column j and the corresponding y.


Documentation
-------------


 Returns the ratio of det Minv_new / det Minv.
 This routine does NOT make any modification. It has to be completed with complete_operation().