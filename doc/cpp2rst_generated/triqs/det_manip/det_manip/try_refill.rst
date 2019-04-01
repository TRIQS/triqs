..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_try_refill:

try_refill
==========

**Synopsis**:

.. code-block:: c

    template<typename ArgumentContainer1, typename ArgumentContainer2>
    triqs::det_manip::det_manip::value_type try_refill (const ArgumentContainer1 & X,
   const ArgumentContainer2 & Y)

Refill determinant with new values


Documentation
-------------


 New values are calculated as f(x_i, y_i)

 Returns the ratio of det Minv_new / det Minv.

 This routine does NOT make any modification. It has to be completed with complete_operation().