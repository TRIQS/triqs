..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__get_x:

triqs::det_manip::det_manip::get_x
==================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | det_manip::x_type const & :red:`get_x` (size_t :param:`i`) const

    2. | std::vector<x_type> :red:`get_x` () const

Documentation



 **1)**   Returns the i-th values of x



 **2)**   Returns a vector with all x_values. Warning : this is slow, since it creates a new copy, and reorders the lines