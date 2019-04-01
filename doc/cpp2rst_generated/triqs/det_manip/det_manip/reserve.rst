..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_reserve:

reserve
=======

**Synopsis**:

.. code-block:: c

    void reserve (size_t new_size)

Like for std::vector, reserve memory for a bigger size.


Documentation
-------------

 Preserves only the matrix, not the temporary working vectors/matrices, so do NOT use it
 between a try_XXX and a complete_operation