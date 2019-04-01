..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_try_remove2:

try_remove2
===========

**Synopsis**:

.. code-block:: c

    triqs::det_manip::det_manip::value_type try_remove2 (size_t i0, size_t i1, size_t j0,
   size_t j1)

Double Removal operation of cols j0,j1 and rows i0,i1


Documentation
-------------


 Returns the ratio of det Minv_new / det Minv.
 This routine does NOT make any modification. It has to be completed with complete_operation().