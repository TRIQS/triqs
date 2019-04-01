..
   Generated automatically by cpp2rst

.. highlight:: c


.. _det_manip_get_x:

get_x
=====

**Synopsis**:

.. code-block:: c

    const triqs::det_manip::det_manip::x_type & get_x (size_t i)     (1)

    std::vector<x_type> get_x ()                                     (2)

(1)Returns the i-th values of x


(2)Returns a vector with all x_values. Warning : this is slow, since it creates a new copy, and reorders the lines