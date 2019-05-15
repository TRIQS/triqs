..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _det_manip_get_x_internal_order:

triqs::det_manip::det_manip::get_x_internal_order
=================================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Advanced: Returns the vector of x_values using the INTERNAL STORAGE ORDER,`
       | std::vector<x_type> const & :red:`get_x_internal_order` () const





   which differs by some permutation from the one given by the user.
 Useful for some performance-critical loops.
 To be used together with other *_internal_order functions.