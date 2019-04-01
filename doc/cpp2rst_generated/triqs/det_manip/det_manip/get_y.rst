..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_get_y:

get_y
=====

**Synopsis**:

.. code-block:: c

    const triqs::det_manip::det_manip::y_type & get_y (size_t j)     (1)

    std::vector<y_type> get_y ()                                     (2)

(1)Returns the j-th values of y


(2)Returns a vector with all y_values. Warning : this is slow, since it creates a new copy, and reorders the cols