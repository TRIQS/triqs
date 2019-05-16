..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__get_y:

triqs::det_manip::det_manip::get_y
==================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | det_manip::y_type const & :red:`get_y` (size_t :param:`j`) const

    2. | std::vector<y_type> :red:`get_y` () const

Documentation



 **1)**   Returns the j-th values of y



 **2)**   Returns a vector with all y_values. Warning : this is slow, since it creates a new copy, and reorders the cols