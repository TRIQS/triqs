..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_get_x_internal_order:

get_x_internal_order
====================

**Synopsis**:

.. code-block:: c

    const std::vector<x_type> & get_x_internal_order ()

Advanced: Returns the vector of x_values using the INTERNAL STORAGE ORDER,


Documentation
-------------

 which differs by some permutation from the one given by the user.
 Useful for some performance-critical loops.
 To be used together with other *_internal_order functions.