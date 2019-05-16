..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip__get_x_internal_order:

triqs::det_manip::det_manip::get_x_internal_order
=================================================

*#include <triqs/det_manip/det_manip.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | std::vector<x_type> const & :red:`get_x_internal_order` () const





Advanced: Returns the vector of x_values using the INTERNAL STORAGE ORDER,
which differs by some permutation from the one given by the user.
Useful for some performance-critical loops.
To be used together with other *_internal_order functions.