..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_try_remove:

try_remove
==========

**Synopsis**:

.. code-block:: c

    triqs::det_manip::det_manip::value_type try_remove (size_t i, size_t j)

Consider the removal the colj0 and row i0 from the matrix.


Documentation
-------------


 Returns the ratio of det Minv_new / det Minv.
 This routine does NOT make any modification. It has to be completed with complete_operation().